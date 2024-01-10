#ifndef BUILTINS_H
#define BUILTINS_H

void init_env(char** envp);
char** get_env(void); // returns envp-style array, DO NOT ATTEMPT TO FREE IT OR ITS CONTENTS
void end_env(void);

int is_builtin(char const* cmd_name);
void builtin(char** cmd);

int is_var_assign(char const* expr);
void var_assign(char const* expr);

#endif // BUILTINS_H
