#ifndef HASHTABLE_H
#define HASHTABLE_H

// dictionary for string to string implemented using hash table

typedef struct htb htb_t;

htb_t* mk_htb(int sz);

// returns NULL if key not found
char const* htb_get(htb_t* htb, char const* key);

void htb_put(htb_t* htb, char const* key, char const* value);

// returns 0 on succesful deletion, 1 if key not found
int htb_delete(htb_t* htb, char const* key);

// returns envp-style array, DO NOT ATTEMPT TO FREE IT OR ITS CONTENTS
char** htb_construct_envp(htb_t* htb);

void free_htb(htb_t* htb);

#endif // HASHTABLE_H
