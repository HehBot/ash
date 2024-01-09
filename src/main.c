#include <lexer.h>
#include <parser.h>
#include <run.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ERR_MSG_CAP 100
static char err_msg[ERR_MSG_CAP];

static int handle_err(void)
{
    if (err_msg[0] == '\0')
        return 0;
    fprintf(stderr, "%s\n", err_msg);
    err_msg[0] = '\0';
    return 1;
}

void post_err(char const* m, ...)
{
    va_list vl;
    va_start(vl, m);
    vsnprintf(err_msg, ERR_MSG_CAP, m, vl);
    va_end(vl);
}

char const* get_prompt(void)
{
    return "$";
}

static char* get_input(FILE* fs)
{
    char* input = NULL;
    size_t len = 0;

    int nr_char = getline(&input, &len, fs);

    if (nr_char == -1)
        return NULL;

    input[nr_char - 1] = '\0';

    return input;
}

int main(int argc, char** argv, char** envp)
{
    init_env(envp);

    while (1) {
        printf("%s ", get_prompt());

        char* input = get_input(stdin);
        if (input == NULL) // EOF or Ctrl-D
            break;

        token_list_t tl = get_tokens(input);
        free(input);

        node_t* n = parse_tokens(tl);
        if (handle_err()) {
            free_run_tree(n);
            free_token_list(tl);
            continue;
        }

        run(n);
        handle_err();

        free_run_tree(n);
        free_token_list(tl);
    }

    return 0;
}
