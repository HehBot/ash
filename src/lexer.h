#ifndef LEXER_H
#define LEXER_H

#include <stddef.h>

typedef enum token_type {
    T_STR, // [^;&|<> \t]+
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

token_list_t get_tokens(char const* input);

void print_token_list(token_list_t tl);
void free_token_list(token_list_t tl);

#endif // LEXER_H
