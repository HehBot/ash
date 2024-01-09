#include <lexer.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char const* typenames[] = {
    "T_STR",
    "T_SEMI",
    "T_AND",
    "T_AMP",
    "T_OR",
    "T_PIPE",
    "T_LAR",
    "T_RAR",
};

static FILE* fs;

static token_list_t mk_token_list(void)
{
#define CAP 5
    token_list_t tl;
    tl.tokens = malloc(CAP * sizeof(tl.tokens[0]));
    tl.nr = 0;
    tl.cap = CAP;
#undef CAP
    return tl;
}
static void append_token_list(token_list_t* tl, token_t t)
{
    if (tl->nr == tl->cap) {
        tl->cap *= 2;
        tl->tokens = realloc(tl->tokens, tl->cap * sizeof(tl->tokens[0]));
    }
    tl->tokens[tl->nr] = t;
    tl->nr++;
}
void print_token_list(token_list_t tl)
{
    printf("Tokens:\n");
    for (int i = 0; i < tl.nr; ++i) {
        printf(" %s", typenames[tl.tokens[i].type]);
        if (tl.tokens[i].type == T_STR)
            printf("[%s]", tl.tokens[i].str);
    }
    printf("\n");
}
void free_token_list(token_list_t tl)
{
    for (int i = 0; i < tl.nr; ++i)
        if (tl.tokens[i].type == T_STR)
            free(tl.tokens[i].str);
    free(tl.tokens);
}

// TODO Implement multi-line commands using '\' at end
static char* get_input(void)
{
    char* input = NULL;
    size_t len = 0;

    int nr_char = getline(&input, &len, fs);
    input[nr_char - 1] = '\0';

    return input;
}

static int is_str_char(char c)
{
    return c != ';' && c != '&' && c != '|' && c != '<' && c != '>' && c != ' ' && c != '\t';
}
static char* scan_quoted_string(char* ptr, int* n)
{
#define CAP 10
    int cap = CAP;
    char* ans = malloc(cap + 1);
#undef CAP
    int len = 0;

    while (*ptr != '"' && *ptr != '\0') {
        if (len >= cap - 1) {
            cap *= 2;
            ans = realloc(ans, cap + 1);
        }
        char c = *ptr;
        ptr++;
        // escapes inside quotes
        if (c == '\\') {
            switch (*ptr) {
            case '"':
                ans[len++] = '"';
                break;
            case '\\':
                ans[len++] = '\\';
                break;
            default:
                ans[len++] = '\\';
                ans[len++] = *ptr;
            }
            ptr++;
        } else
            ans[len++] = c;
    }

    ans[len] = '\0';
    *n = len;
    return ans;
}
static char* scan_string(char* ptr, int* n)
{
#define CAP 10
    int cap = CAP;
    char* ans = malloc(cap + 1);
#undef CAP
    int len = 0;
    *n = 0;

    while (*ptr != '\0' && (is_str_char(*ptr) || *ptr == '\\')) {
        if (len >= cap - 1) {
            cap *= 2;
            ans = realloc(ans, cap + 1);
        }
        char c = *ptr;
        ptr++;
        // escapes outside quotes
        if (c == '\\') {
            ans[len++] = *ptr;
            *n += 1;
            ptr++;
        } else if (c == '"') {
            int quoted_len;
            char* quoted_str = scan_quoted_string(ptr, &quoted_len);
            if (quoted_len > 0) {
                if (len + quoted_len > cap) {
                    cap += quoted_len;
                    ans = realloc(ans, cap + 1);
                }
                ans[len] = '\0';
                strcat(ans, quoted_str);
                len += quoted_len;
                ptr += quoted_len;
                *n += 2;
            }
            free(quoted_str);
            ptr++;
        } else
            ans[len++] = c;
    }

    ans[len] = '\0';
    *n += len;
    return ans;
}

void init_lexer(FILE* _fs)
{
    fs = _fs;
}
token_list_t get_tokens(void)
{
    char* line = get_input();
    token_list_t tl = mk_token_list();

    char* ptr = line;
    while (*ptr != '\0') {
        token_t t = { 0, NULL };
        switch (*ptr) {
        case ';':
            t.type = T_SEMI;
            break;
        case '&':
            if (*(ptr + 1) == '&') {
                t.type = T_AND;
                ptr++;
            } else
                t.type = T_AMP;
            break;
        case '|':
            if (*(ptr + 1) == '|') {
                t.type = T_OR;
                ptr++;
            } else
                t.type = T_PIPE;
            break;
        case '<':
            t.type = T_LAR;
            break;
        case '>':
            t.type = T_RAR;
            break;
        case '"':
            t.type = T_STR;
            {
                ptr++;
                int n;
                t.str = scan_quoted_string(ptr, &n);
                ptr += n;
            }
            break;
        case ' ':
        case '\t':
            // skip over whitespaces
            while (*ptr == ' ' || *ptr == '\t')
                ptr++;
            continue;
        default:
            t.type = T_STR;
            {
                int n;
                t.str = scan_string(ptr, &n);
                ptr += n;
                ptr--;
            }
            break;
        }
        append_token_list(&tl, t);
        ptr++;
    }
    free(line);
    return tl;
}
