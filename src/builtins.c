#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
            // TODO change PWD
        }
        break;
    case 'e':
        if (strcmp(cmd[0], "exit") == 0)
            exit(0);
        break;
    }
}
