#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

typedef enum token_type {
    T_STR, // [a-zA-Z0-9-_/.]+
    T_SEMI, // ;
    T_AND, // &&
    T_AMP, // &
    T_OR, // ||
    T_PIPE, // |
    T_LAR, // <
    T_RAR, // >
} token_type_t;

typedef struct token {
    token_type_t type;
    char* str;
} token_t;

typedef struct token_list {
    token_t* tokens;
    size_t nr, cap;
} token_list_t;

void init_lexer(FILE* f);
token_list_t get_tokens(void);

void print_token_list(token_list_t tl);
void free_token_list(token_list_t tl);

#endif // LEXER_H
