#ifndef RUN_H
#define RUN_H

typedef struct node node_t;
void run(node_t* tree);
void free_run_tree(node_t* tree);

typedef struct htb htb_t;
extern struct {
    htb_t* export;
    htb_t* local;
} env;

#endif // RUN_H
