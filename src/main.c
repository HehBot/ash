#include <lexer.h>
#include <run.h>
#include <stdio.h>

char const* get_prompt(void)
{
    return "$";
}

int main(int argc, char** argv, char** envp)
{
    init_lexer(stdin);
    init_env(envp);

    while (1) {
        printf("%s ", get_prompt());

        token_list_t tl = get_tokens();
        node_t* n = parse_tokens(tl);
        run(n);

        free_run_tree(n);
        free_token_list(tl);
    }
}
