#include "compiler.h"

// NOTE: assumes AST is syntactically correct

static FILE* file;
static Var* vars;
static Var* varsHead;
static size_t stackPointer;

static void push_var(size_t stackOffset, String id)
{
    vars->next = (Var*)malloc(sizeof(Var));
    vars->next->stackOffset = stackOffset;
    vars->next->id = id;
    vars->next->next = NULL;

    vars = vars->next;
}

static Var* find_var(String id)
{
    Var* current = varsHead->next;
    while (current)
    {
        if (strcmp(id.str, current->id.str) == 0)
        {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

static void gen_expr(Node* exprNode)
{
    if (exprNode->firstChild->type == NODE_OP_ADD)
    {
        Node* addNode = exprNode->firstChild;
        Node* leftIntNode = addNode->firstChild;
        Node* rightIntNode = leftIntNode->nextSibling;

        int leftInt = atoi(leftIntNode->string.str);
        int rightInt = atoi(rightIntNode->string.str);

        fprintf(file, "    mov eax, %i\n", leftInt);
        fprintf(file, "    mov ebx, %i\n", rightInt);
        fprintf(file, "    add eax, ebx\n");

        return;
    }

    if (exprNode->firstChild->type == NODE_INT)
    {
        Node* intNode = exprNode->firstChild;
        int exitCode = atoi(intNode->string.str);
        fprintf(file, "    mov eax, %i\n", exitCode);
        return;
    }

    if (exprNode->firstChild->type == NODE_ID)
    {
        Node* idNode = exprNode->firstChild;
        Var* var = find_var(idNode->string);

        if (!var)
        {
            printf("%s is not declared!\n", idNode->string.str);
            exit(-1);
        }

        fprintf(file, "    mov eax, dword [rbp - %llu]\n", var->stackOffset);
        return;
    }
}

static void gen_var(Node* varNode)
{
    stackPointer += 4;
    Node* idNode = varNode->firstChild;
    push_var(stackPointer, idNode->string);

    // HACK: puts the int in eax
    Node* exprNode = idNode->nextSibling->firstChild;
    gen_expr(exprNode);
    fprintf(file, "    mov dword [rbp - %llu], eax\n", stackPointer);
}

static void gen_fn(Node* fnNode)
{
    Node* idNode = fnNode->firstChild;

    fprintf(file, "\nglobal %s\n", idNode->string.str);
    fprintf(file, "%s:\n", idNode->string.str);
    fprintf(file, "    push rbp\n");
    fprintf(file, "    mov rbp, rsp\n\n");

    Node* current = idNode->nextSibling;
    while (current)
    {
        if (current->type == NODE_RETURN)
        {
            Node* exprNode = current->firstChild;
            gen_expr(exprNode);
            fputc('\n', file);
        }

        if (current->type == NODE_VAR)
        {
            Node* varNode = current;
            gen_var(varNode);
            fputc('\n', file);
        }

        current = current->nextSibling;
    }

    fprintf(file, "    pop rbp\n");
    fprintf(file, "    ret\n");
}

void generate(Node* tree, const char* path)
{
    vars = (Var*)malloc(sizeof(Var));
    memset(vars, 0, sizeof(Var));
    varsHead = vars;
    stackPointer = 0;

    file = fopen(path, "w");
    if (!file)
    {
        printf("couldn't open file: %s\n", path);
        return;
    }

    Node* current = tree;

    if (current->type != NODE_PROGRAM)
    {
        goto exit;
    }
    current = current->firstChild;

    // print header
    fprintf(file, "bits 64\n");
    fprintf(file, "default rel\n\n");
    fprintf(file, "section .text\n");

    while (current)
    {
        if (current->type == NODE_FN)
        {
            gen_fn(current);
        }

        current = current->nextSibling;
    }

exit:
    fclose(file);
}