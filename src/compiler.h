#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))

typedef struct String
{
    char* str;
    size_t len;
} String;

#define STRLIT(s) ((String){.str = (s), .len = strlen((s))})

String string_new(const char* str);
char* read_file(const char* path);

typedef enum TokenType
{
    TOKEN_INT,

    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_SEMI,

    TOKEN_PLUS,
    TOKEN_EQUALS,

    TOKEN_RETURN,
    TOKEN_FN,
    TOKEN_LET,

    TOKEN_ID,
} TokenType;

typedef struct Token
{
    TokenType type;
    String string;
    struct Token* next;
} Token;

Token* tokenize(char* contents);

typedef enum NodeType
{
    NODE_PROGRAM,

    NODE_INT,

    NODE_EXPR,
    NODE_TERM,

    NODE_OP_ADD,
    NODE_OP_ASSIGN,

    NODE_RETURN,
    NODE_LET,
    NODE_FN,
    NODE_VAR,
    NODE_FN_CALL,

    NODE_ID,
} NodeType;

typedef struct Node
{
    NodeType type;
    String string;
    struct Node* nextSibling;
    struct Node* firstChild;
} Node;

Node* parse(Token* tokens);

typedef struct Var 
{
    size_t stackOffset;
    String id;
    struct Var* next;
} Var;

void generate(Node* tree, const char* path);