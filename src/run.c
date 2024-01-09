#define _GNU_SOURCE
#include <builtins.h>
#include <err.h>
#include <fcntl.h>
#include <parser.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

void free_run_tree(node_t* n)
{
    while (n != NULL) {
        node_t* next = n->next;
        if (n->in != NULL && n->in->fd != -1)
            close(n->in->fd);
        free(n->in);
        if (n->out != NULL && n->out->fd != -1)
            close(n->out->fd);
        free(n->out);
        free(n->cmd);
        free(n);
        n = next;
    }
}

static int handle_pipe(stream_t* read_end, stream_t* write_end)
{
    int pipefd[2];
    if (pipe(pipefd) == -1) {
        post_err("pipe failed");
        return 0;
    }
    read_end->fd = pipefd[0];
    write_end->fd = pipefd[1];
    return 1;
}

int handle_streams(node_t* n)
{
    stream_t* s = n->in;
    if (s != NULL && s->fd == -1) {
        switch (s->type) {
        case S_PIPE:
            if (!handle_pipe(s, s->pipe_other_end))
                return 0;
            break;
        case S_FILE:
            if ((s->fd = open(s->filename, O_RDONLY)) == -1) {
                post_err("unable to open file '%s' for reading redirection", s->filename);
                return 0;
            }
        }
    }

    s = n->out;
    if (s != NULL && s->fd == -1) {
        switch (s->type) {
        case S_PIPE:
            if (!handle_pipe(s->pipe_other_end, s))
                return 0;
            break;
        case S_FILE:
            if ((s->fd = open(s->filename, O_WRONLY | O_CREAT | O_TRUNC, 0644)) == -1) {
                post_err("unable to open file '%s' for writing redirection", s->filename);
                return 0;
            }
        }
    }

    return 1;
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
        if (!handle_streams(n))
            return;
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
                if (n->in != NULL) {
                    close(n->in->fd);
                    n->in->fd = -1;
                }
                if (n->out != NULL) {
                    close(n->out->fd);
                    n->out->fd = -1;
                }
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
