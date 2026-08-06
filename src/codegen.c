#include "compiler.h"

// NOTE: assumes AST is 100% correct

static FILE* file;

static void gen_expr(Node* exprNode)
{
    if (exprNode->firstChild->type == NODE_OP_ADD)
    {
        Node* addNode = exprNode->firstChild;
        Node* leftIntNode = addNode->firstChild;
        Node* rightIntNode = leftIntNode->nextSibling;

        int leftInt = atoi(leftIntNode->string.str);
        int rightInt = atoi(rightIntNode->string.str);

        fprintf(file, "\n    mov eax, %i\n", leftInt);
        fprintf(file, "    mov ebx, %i\n", rightInt);
        fprintf(file, "    add eax, ebx\n\n");
    }
    else
    {
        Node* intNode = exprNode->firstChild;
        int exitCode = atoi(intNode->string.str);
        fprintf(file, "\n    mov eax, %i\n\n", exitCode);
    }
}

static void gen_fn(Node* fnNode)
{
    Node* idNode = fnNode->firstChild;

    fprintf(file, "global %s\n", idNode->string.str);
    fprintf(file, "%s:\n", idNode->string.str);
    fprintf(file, "    push rbp\n");
    fprintf(file, "    mov rbp, rsp\n");

    if (strcmp(idNode->string.str, "main") == 0)
    {
        fprintf(file, "    sub rsp, 32\n");
    }

    Node* current = idNode->nextSibling;
    while (current)
    {
        if (current->type == NODE_RETURN)
        {
            Node* exprNode = current->firstChild;
            gen_expr(exprNode);
        }

        current = current->nextSibling;
    }

    if (strcmp(idNode->string.str, "main") == 0)
    {
        fprintf(file, "    add rsp, 32\n");
    }
    fprintf(file, "    pop rbp\n");
    fprintf(file, "    ret\n\n");
}

void generate(Node* tree, const char* path)
{
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
    fprintf(file, "section .text\n\n");

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