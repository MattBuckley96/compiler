#include "compiler.h"

// NOTE: assumes AST is 100% correct

static FILE* file;
static Var* vars;
static size_t stackPointer;

static void push_var(size_t stackOffset, String id)
{
    vars->next = (Var*)malloc(sizeof(Var));
    vars->next->stackOffset = stackOffset;
    vars->next->id = id;
    vars->next->next = NULL;

    vars = vars->next;
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

        fprintf(file, "    mov ebx, %i\n", leftInt);
        fprintf(file, "    mov ecx, %i\n", rightInt);
        fprintf(file, "    add ebx, ecx\n");
        fprintf(file, "    mov eax, ebx\n");
    }
    else
    {
        Node* intNode = exprNode->firstChild;
        int exitCode = atoi(intNode->string.str);
        fprintf(file, "    mov eax, %i\n", exitCode);
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