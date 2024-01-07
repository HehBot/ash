#include <parser.h>
#include <stdio.h>
#include <stdlib.h>

static void parse_error(void)
{
    fprintf(stderr, "Bad command");
    exit(1);
}

int parse_file_redirects(token_list_t tl, int start, node_t* n)
{
    if (start == tl.nr)
        return start;
    else if (tl.tokens[start].type == T_LAR) {
        if (start == tl.nr - 1)
            parse_error();
        stream_t* s = malloc(sizeof(*s));
        s->type = S_FILE;
        s->filename = tl.tokens[start + 1].str;
        s->fd = -1;
        if (n->in != NULL)
            parse_error();
        n->in = s;
        return parse_file_redirects(tl, start + 2, n);
    } else if (tl.tokens[start].type == T_RAR) {
        if (start == tl.nr - 1)
            parse_error();
        stream_t* s = malloc(sizeof(*s));
        s->type = S_FILE;
        s->filename = tl.tokens[start + 1].str;
        s->fd = -1;
        if (n->out != NULL)
            parse_error();
        n->out = s;
        return parse_file_redirects(tl, start + 2, n);
    } else
        return start;
}

node_t* parse_basic_command(token_list_t tl, int* start)
{
    int i = *start;
    if (i == tl.nr)
        return NULL;
    for (; i < tl.nr && tl.tokens[i].type == T_STR; ++i)
        ;

    char** cmd = malloc((i - *start + 1) * sizeof(cmd[0]));
    int k = 0;
    for (int j = *start; j < i; ++j) {
        if (tl.tokens[j].type == T_STR) {
            cmd[k] = tl.tokens[j].str;
            ++k;
        }
    }
    cmd[k] = NULL;

    node_t* n = malloc(sizeof(*n));
    n->type = N_EXEC;
    n->cmd = cmd;
    n->in = n->out = NULL;
    n->next = NULL;

    i = parse_file_redirects(tl, i, n);

    if (i != tl.nr && tl.tokens[i].type == T_PIPE) {
        ++i;
        node_t* rest = parse_basic_command(tl, &i);
        if (rest == NULL)
            parse_error();
        if (n->out != NULL || rest->in != NULL)
            parse_error();
        n->next = rest;
        {
            stream_t* s1 = malloc(sizeof(*s1));
            stream_t* s2 = malloc(sizeof(*s2));

            s1->type = S_PIPE;
            s1->pipe_other_end = s2;
            s1->fd = -1;
            s2->type = S_PIPE;
            s2->pipe_other_end = s1;
            s2->fd = -1;

            n->out = s1;
            rest->in = s2;
        }
    }

    return n;
}

node_t* parse_tokens(token_list_t tl)
{
    int n = 0;
    return parse_basic_command(tl, &n);
}

void free_run_tree(node_t* n)
{
    while (n != NULL) {
        node_t* next = n->next;
        free(n->in);
        free(n->out);
        free(n);
        n = next;
    }
}
