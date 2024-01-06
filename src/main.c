#include <lexer.h>
#include <stdio.h>

char const* get_prompt(void)
{
    return "$";
}

int main(int argc, char** argv, char** envp)
{
    init_lexer(stdin);

    while (1) {
        printf("%s ", get_prompt());
        token_list_t tl = get_tokens();
        print_token_list(&tl);
        free_token_list(&tl);
    }
}
