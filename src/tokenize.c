#include "compiler.h"

static Token* tokens;

static void push_token(TokenType type, String string)
{
    tokens->next = (Token*)malloc(sizeof(Token));
    tokens->next->type = type;
    tokens->next->string = string;
    tokens->next->next = NULL;

    tokens = tokens->next;
}

Token* tokenize(char* contents)
{
    tokens = (Token*)malloc(sizeof(Token));
    memset(tokens, 0, sizeof(Token));

    Token* head = tokens;

    for (size_t i = 0; i < strlen(contents); i++)
    {
        char c = contents[i];

        // skip whitespace
        if (c == ' ' || c == '\n')
        {
            continue;
        }

        // check for separators
        if (c == '(')
        {
            push_token(TOKEN_LPAREN, STRLIT("("));
            continue;
        }
        if (c == ')')
        {
            push_token(TOKEN_RPAREN, STRLIT(")"));
            continue;
        }
        if (c == ';')
        {
            push_token(TOKEN_SEMI, STRLIT(";"));
            continue;
        }
        if (c == '+')
        {
            push_token(TOKEN_PLUS, STRLIT("+"));
            continue;
        }

        // get integers
        if (isdigit(c))
        {
            char digits[32];
            size_t pos = 0;

            while (isdigit(c))
            {
                digits[pos++] = c;
                i++;
                c = contents[i];
            }
            i--;
            digits[pos] = '\0';

            push_token(TOKEN_INT, string_new(digits));
            continue;
        }

        // get keywords
        char buf[256];
        size_t pos = 0;

        while (isalnum(c))
        {
            buf[pos++] = c;
            i++;
            c = contents[i];
        }
        i--;
        buf[pos] = '\0';

        if (strcmp(buf, "exit") == 0)
        {
            push_token(TOKEN_EXIT, string_new(buf));
            continue;
        }

        push_token(TOKEN_ID, string_new(buf));
    }

    return head;
}