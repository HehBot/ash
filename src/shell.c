#include <ds/htb.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static char* split_eq(char* s)
{
    char* value = s;
    while (*value != '=' && *value != '\0')
        value++;
    if (*value == '\0')
        return NULL;
    *value = '\0';
    value++;
    return value;
}

static struct {
    htb_t* export;
    htb_t* local;
} env;

void init_env(char** envp)
{
    env.export = mk_htb(30);
    env.local = mk_htb(30);
    for (int i = 0; envp[i] != NULL; ++i) {
        char* key = strdup(envp[i]);
        char* value = split_eq(key);
        htb_put(env.export, key, value);
        free(key);
    }
}
char** get_env(void)
{
    return htb_construct_envp(env.export);
}
void end_env(void)
{
    free_htb(env.export);
    free_htb(env.local);
}

int is_var_assign(char const* expr)
{
    char* key = strdup(expr);
    char* val = split_eq(key);

    free(key);
    return (val != NULL);
}
void var_assign(char const* expr)
{
    char* key = strdup(expr);
    char* val = split_eq(key);

    htb_put(env.export, key, val);

    free(key);
}

int is_builtin(char const* cmd_name)
{
    switch (cmd_name[0]) {
    case 'c':
        if (strcmp(cmd_name, "cd") == 0)
            return 1;
        break;
    case 'e':
        if (strcmp(cmd_name, "exit") == 0)
            return 1;
        break;
    }
    return 0;
}
void builtin(char** cmd)
{
    switch (cmd[0][0]) {
    case 'c':
        if (strcmp(cmd[0], "cd") == 0) {
            chdir(cmd[1]);
#define CAP 50
            int sz = CAP;
            char* buf = malloc(sz);
#undef CAP
            char* cwd = getcwd(buf, sz);
            while (cwd == NULL) {
                sz *= 2;
                free(buf);
                buf = malloc(sz);
                cwd = getcwd(buf, sz);
            }
            htb_put(env.export, "PWD", cwd);
            free(buf);
        }
        break;
    case 'e':
        if (strcmp(cmd[0], "exit") == 0)
            exit(0);
        break;
    }
}
