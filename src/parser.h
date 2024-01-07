#ifndef PARSER_H
#define PARSER_H

#include <lexer.h>

typedef enum stream_type {
    S_FILE,
    S_PIPE,
} stream_type_t;

typedef struct stream {
    stream_type_t type;
    union {
        char* filename;
        struct stream* pipe_other_end;
    };
    int fd;
} stream_t;

typedef enum node_type {
    N_EXEC,
    N_CHAIN,
    N_SUCC,
    N_FAIL,
} node_type_t;

typedef struct node {
    node_type_t type;
    union {
        //         struct {
        //             struct node* left;
        //             struct node* right;
        //         };
        struct {
            char** cmd;
            stream_t* in;
            stream_t* out;
            struct node* next;
            int pid;
        };
    };
} node_t;

node_t* parse_tokens(token_list_t tl);
void free_run_tree(node_t* n);

#endif // PARSER_H
