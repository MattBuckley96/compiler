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

static void parse_term(Node* root)
{
    Node* termNode = add_child(root, NODE_TERM, STRLIT("Term"));

    if (check_next_and_consume(TOKEN_INT))
    {
        add_child(termNode, NODE_INT, list->string);
        return;
    }

    if (check_next_and_consume(TOKEN_ID))
    {
        if (check_next(TOKEN_LPAREN))
        {
            Node* fnCallNode = add_child(termNode, NODE_FN_CALL, STRLIT("FunctionCall"));
            add_child(fnCallNode, NODE_ID, list->string);

            check_next_and_consume(TOKEN_LPAREN);
            check_next_and_consume(TOKEN_RPAREN);
        }
        else
        {
            add_child(termNode, NODE_ID, list->string);
        }

        return;
    }

    printf("Invalid term!\n");
    exit(-1);
}

static void parse_expr(Node* root)
{
    Node* exprNode = add_child(root, NODE_EXPR, STRLIT("Expr"));

    // TODO: refactor this bs 
    Token* current = list->next->next;
    bool plusFound = false;

    while (current)
    {
        if (current->type == TOKEN_PLUS)
        {
            plusFound = true;
            break;
        }

        if (current->type == TOKEN_SEMI)
        {
            break;
        }

        current = current->next;
    }

    if (plusFound)
    {
        Node* addNode = add_child(exprNode, NODE_OP_ADD, STRLIT("+"));

        // left
        parse_term(addNode);

        check_next_and_consume(TOKEN_PLUS);

        // right
        parse_term(addNode);
    }
    else
    {
        parse_term(exprNode);
    }

    if (!check_next(TOKEN_SEMI))
    {
        printf("End of statement needs ';'!\n");
        exit(-1);
    }
}

static void parse_var(Node* root)
{
    if (!check_next_and_consume(TOKEN_ID))
    {
        printf("Variable needs Identifier!\n");
        exit(-1);
    }

    Node* idNode = add_child(root, NODE_ID, list->string);

    if (!check_next_and_consume(TOKEN_EQUALS))
    {
        printf("Unassigned variable!\n");
        exit(-1);
    }

    Node* equalsNode = add_child(root, NODE_OP_ASSIGN, STRLIT("="));
    parse_expr(equalsNode);
}

static void parse_fn(Node* root)
{
    if (!check_next_and_consume(TOKEN_ID))
    {
        printf("Function needs Identifier!\n");
        exit(-1);
    }

    Node* idNode = add_child(root, NODE_ID, list->string);

    check_next_and_consume(TOKEN_LPAREN);
    check_next_and_consume(TOKEN_RPAREN);
    check_next_and_consume(TOKEN_LBRACE);

    while (!check_next(TOKEN_RBRACE))
    {
        if (check_next_and_consume(TOKEN_LET))
        {
            Node* letNode = add_child(root, NODE_LET, STRLIT("Let"));
            parse_var(letNode);
            continue;
        }

        if (check_next(TOKEN_ID))
        {
            Node* varNode = add_child(root, NODE_VAR, STRLIT("Variable"));
            parse_var(varNode);
            continue;
        }

        if (check_next_and_consume(TOKEN_RETURN))
        {
            Node* returnNode = add_child(root, NODE_RETURN, STRLIT("Return"));
            parse_expr(returnNode);
            continue;
        }

        list = list->next;
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

            parse_fn(fnNode);
        }

        list = list->next;
    }

    return root;
}