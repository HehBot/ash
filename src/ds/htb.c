#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// see https://cseweb.ucsd.edu/~kube/cls/100/Lectures/lec16/lec16-16.html
int hash(char const* key)
{
    int hashval = 0;
    while (*key != '\0') {
        hashval = (hashval << 4) + *(key++);
        int g = hashval & 0xf0000000;
        if (g != 0)
            hashval ^= (g >> 24);
        hashval &= ~g;
    }
    return hashval;
}

typedef struct node {
    char* key;
    char* value;
    struct node* next;

    char* key_value;
} node_t;

typedef struct htb {
    node_t** nodes;
    int sz;

    char** envp;
    int envp_cap;
    enum { FRESH,
           STALE } envp_state;
} htb_t;

htb_t* mk_htb(int sz)
{
    node_t** nodes = malloc(sz * sizeof(nodes[0]));
    memset(nodes, 0, sz * sizeof(nodes[0])); // setting all nodes to empty
    htb_t* htb = malloc(sizeof(*htb));
    htb->nodes = nodes;
    htb->sz = sz;
#define CAP 20
    htb->envp = malloc(CAP * sizeof(htb->envp[0]));
    htb->envp_cap = CAP;
#undef CAP
    return htb;
}

char const* htb_get(htb_t* htb, char const* key)
{
    int i = hash(key) % htb->sz;

    node_t* n = htb->nodes[i];
    while (n != NULL) {
        if (!strcmp(n->key, key))
            return n->value;
        n = n->next;
    }
    return NULL;
}

void htb_put(htb_t* htb, char const* key, char const* value)
{
    int i = hash(key) % htb->sz;

    int found = 0;
    node_t* n = htb->nodes[i];
    if (n == NULL) {
        n = htb->nodes[i] = malloc(sizeof(*n));
        n->key = strdup(key);
        n->next = NULL;
    } else {
        node_t* next = n->next;
        while (next != NULL) {
            if (!strcmp(n->key, key)) {
                found = 1;
                break;
            }
            n = next;
            next = next->next;
        }
        if (!found && !strcmp(n->key, key))
            found = 1;

        if (!found) {
            n->next = malloc(sizeof(*n->next));
            n = n->next;
            n->next = NULL;
            n->key = strdup(key);
        } else
            free(n->value);
    }
    n->value = strdup(value);
    n->key_value = malloc(strlen(key) + 1 + strlen(value) + 1);
    sprintf(n->key_value, "%s=%s", key, value);

    htb->envp_state = STALE;
}

int htb_delete(htb_t* htb, char const* key)
{
    int i = hash(key) % htb->sz;

    // key is in very first node
    node_t* n = htb->nodes[i];
    if (n == NULL)
        return 0;
    else if (!strcmp(n->key, key)) {
        htb->nodes[i] = n->next;
        goto successful;
    }

    node_t* prev = n;
    n = n->next;
    while (n != NULL) {
        if (!strcmp(n->key, key))
            break;
        prev = n;
        n = n->next;
    }

    if (n == NULL)
        return 0;

    prev->next = n->next;

successful:
    free(n->key);
    free(n->value);
    free(n->key_value);
    free(n);
    htb->envp_state = STALE;
    return 1;
}

char** htb_construct_envp(htb_t* htb)
{
    if (htb->envp_state == FRESH)
        return htb->envp;

    int nrvars = 0;

    for (int i = 0; i < htb->sz; ++i) {
        node_t* n = htb->nodes[i];
        while (n != NULL) {
            if (nrvars + 1 > htb->envp_cap) {
                htb->envp_cap *= 2;
                htb->envp = realloc(htb->envp, htb->envp_cap * sizeof(htb->envp[0]));
            }
            htb->envp[nrvars] = n->key_value;
            nrvars++;
            n = n->next;
        }
    }
    htb->envp[nrvars] = NULL;
    htb->envp_state = FRESH;
    return htb->envp;
}

void free_htb(htb_t* htb)
{
    for (int i = 0; i < htb->sz; ++i) {
        node_t* n = htb->nodes[i];
        while (n != NULL) {
            free(n->key);
            free(n->value);
            free(n->key_value);
            node_t* nn = n->next;
            free(n);
            n = nn;
        }
    }
    free(htb->nodes);
    free(htb->envp);
    free(htb);
}
