#include "compiler.h"

static Token* list;
static Node* tree;

static Node* add_child(Node* parent, NodeType type, String string)
{
    Node* child = malloc(sizeof(Node));
    child->firstChild = NULL;
    child->nextSibling = NULL;
    child->type = type;
    child->string = string;

    if (!parent || !child)
    {
        return NULL;
    }

    if (!parent->firstChild)
    {
        parent->firstChild = child;
    }
    else
    {
        Node* current = parent->firstChild;
        while (current->nextSibling)
        {
            current = current->nextSibling;
        }
        current->nextSibling = child;
    }

    return child;
}

static bool check_next(TokenType type)
{
    if (!(list->next))
    {
        return false;
    }

    return (list->next->type == type);
}

static bool check_next_and_consume(TokenType type)
{
    if (!(list->next))
    {
        return false;
    }

    if (list->next->type == type)
    {
        list = list->next;
        return true;
    }

    return false;
}

static void parse_expr(Node* root)
{
    if (!check_next_and_consume(TOKEN_LPAREN))
    {
        printf("Expression needs a '('!\n");
        exit(0);
    }
    Node* exprNode = add_child(root, NODE_EXPR, STRLIT("Expr"));

    if (!check_next_and_consume(TOKEN_INT))
    {
        printf("Expression must have an int!\n");
        exit(0);
    }
    Token* leftIntToken = list;

    if (check_next_and_consume(TOKEN_PLUS))
    {
        Node* addNode = add_child(exprNode, NODE_OP_ADD, STRLIT("+"));
        if (!check_next_and_consume(TOKEN_INT))
        {
            printf("Operation must have two ints!\n");
            exit(0);
        }
        Token* rightIntToken = list;

        add_child(addNode, NODE_INT, leftIntToken->string);
        add_child(addNode, NODE_INT, rightIntToken->string);
    }
    else
    {
        add_child(exprNode, NODE_INT, leftIntToken->string);
    }

    if (!check_next_and_consume(TOKEN_RPAREN) || !check_next(TOKEN_SEMI))
    {
        printf("Parsing error!\n");
        exit(0);
    }
}

Node* parse(Token* tokens)
{
    tree = (Node*)malloc(sizeof(Node));
    memset(tree, 0, sizeof(Node));
    tree->type = NODE_PROGRAM;
    tree->string = STRLIT("Program");

    Node* root = tree;
    list = tokens;

    while (list)
    {
        if (check_next_and_consume(TOKEN_RETURN))
        {
            Node* returnNode = root;
            returnNode = add_child(root, NODE_RETURN, STRLIT("Return"));

            parse_expr(returnNode);
        }

        list = list->next;
    }

    return root;
}