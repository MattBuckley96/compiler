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
    // consume paren if there
    check_next_and_consume(TOKEN_LPAREN);

    Node* exprNode = add_child(root, NODE_EXPR, STRLIT("Expr"));

    if (!check_next_and_consume(TOKEN_INT))
    {
        printf("Expression must have an int!\n");
        exit(-1);
    }
    Token* leftIntToken = list;

    if (check_next_and_consume(TOKEN_PLUS))
    {
        Node* addNode = add_child(exprNode, NODE_OP_ADD, STRLIT("+"));
        if (!check_next_and_consume(TOKEN_INT))
        {
            printf("Addition needs two ints!\n");
            exit(-1);
        }
        Token* rightIntToken = list;

        add_child(addNode, NODE_INT, leftIntToken->string);
        add_child(addNode, NODE_INT, rightIntToken->string);
    }
    else
    {
        add_child(exprNode, NODE_INT, leftIntToken->string);
    }

    check_next_and_consume(TOKEN_RPAREN);

    if (!check_next(TOKEN_SEMI))
    {
        printf("End of statement needs ';'!\n");
        exit(-1);
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
        if (check_next_and_consume(TOKEN_FN))
        {
            Node* fnNode = root;
            fnNode = add_child(root, NODE_FN, STRLIT("Function"));

            if (!check_next_and_consume(TOKEN_ID))
            {
                printf("Function needs Identifier!\n");
                exit(-1);
            }

            Node* idNode = add_child(fnNode, NODE_ID, list->string);

            check_next_and_consume(TOKEN_LPAREN);
            check_next_and_consume(TOKEN_RPAREN);
            check_next_and_consume(TOKEN_LBRACE);

            if (check_next_and_consume(TOKEN_RETURN))
            {
                Node* returnNode = add_child(fnNode, NODE_RETURN, STRLIT("Return"));
                parse_expr(returnNode);
            }

            check_next_and_consume(TOKEN_RBRACE);
        }

        list = list->next;
    }

    return root;
}