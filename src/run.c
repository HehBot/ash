#define _GNU_SOURCE
#include <builtins.h>
#include <fcntl.h>
#include <parser.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void sem_error(char const* s, ...)
{
    fprintf(stderr, "ash: Error: ");
    va_list argptr;
    va_start(argptr, s);
    vfprintf(stderr, s, argptr);
    exit(1);
}

struct {
    char** global;
    //     dict_t* local;
} env;

void init_env(char** envp)
{
    env.global = envp;
    //     local = NULL;
}

void run(node_t* tree)
{
    node_t* n = tree;

    while (n != NULL) {
        if (n->in != NULL && n->in->fd == -1) {
            if (n->in->type == S_PIPE) {
                int pipefd[2];
                if (pipe(pipefd) == -1)
                    sem_error("pipe failed");
                n->in->fd = pipefd[0];
                n->in->pipe_other_end->fd = pipefd[1];
            } else {
                if ((n->in->fd = open(n->in->filename, O_RDONLY)) == -1)
                    sem_error("unable to open file %s for reading", n->in->filename);
            }
        } else if (n->out != NULL && n->out->fd == -1) {
            if (n->out->type == S_PIPE) {
                int pipefd[2];
                if (pipe(pipefd) == -1)
                    sem_error("pipe failed");
                n->out->fd = pipefd[1];
                n->out->pipe_other_end->fd = pipefd[0];
            } else {
                if ((n->out->fd = open(n->out->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1)
                    sem_error("unable to open file %s for writing", n->out->filename);
            }
        }
        n = n->next;
    }

    n = tree;
    while (n != NULL) {
        if (is_builtin(n->cmd[0]))
            builtin(n->cmd);
        else {
            int p = fork();
            if (p == 0) {
                if (n->in != NULL) {
                    dup2(n->in->fd, STDIN_FILENO);
                    close(n->in->fd);
                }
                if (n->out != NULL) {
                    dup2(n->out->fd, STDOUT_FILENO);
                    close(n->out->fd);
                }
                execvpe(n->cmd[0], n->cmd, env.global);
            } else {
                if (n->in != NULL)
                    close(n->in->fd);
                if (n->out != NULL)
                    close(n->out->fd);
                n->pid = p;
            }
        }
        n = n->next;
    }

    n = tree;

    while (n != NULL) {
        waitpid(n->pid, NULL, 0);
        n = n->next;
    }
}
