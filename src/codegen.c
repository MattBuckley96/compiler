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

        fprintf(file, "    mov eax, %i\n", leftInt);
        fprintf(file, "    mov ebx, %i\n", rightInt);
        fprintf(file, "    add eax, ebx\n");
    }
    else
    {
        Node* intNode = exprNode->firstChild;
        int exitCode = atoi(intNode->string.str);
        fprintf(file, "    mov eax, %i\n", exitCode);
    }
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
    fprintf(file, "default rel\n");
    fprintf(file, "section .text\n");
    fprintf(file, "global main\n");
    fprintf(file, "main:\n");
    fprintf(file, "    push rbp\n");
    fprintf(file, "    mov rbp, rsp\n");
    fprintf(file, "    sub rsp, 32\n");

    while (current)
    {
        if (current->type == NODE_RETURN)
        {
            Node* exprNode = current->firstChild;
            gen_expr(exprNode);
        }

        current = current->nextSibling;
    }

    // print footer
    fprintf(file, "    add rsp, 32\n");
    fprintf(file, "    pop rbp\n");
    fprintf(file, "    ret\n");

exit:
    fclose(file);
}