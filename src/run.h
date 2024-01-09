#ifndef RUN_H
#define RUN_H

void init_env(char** envp);

typedef struct node node_t;
void run(node_t* tree);
void free_run_tree(node_t* tree);

#endif // RUN_H
