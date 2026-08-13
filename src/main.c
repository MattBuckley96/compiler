#include "compiler.h"

String string_new(const char* str)
{
    size_t len = strlen(str);

    String string = {
        .str = malloc(len + 1),
        .len = len,
    };
    strcpy(string.str, str);
    string.str[len + 1] = '\0';

    return string;
}

char* read_file(const char* path)
{
    FILE* file = fopen(path, "r");
    if (!file)
    {
        printf("couldn't open file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    size_t size = ftell(file);
    rewind(file);

    char* buffer = malloc(size + 1);
    if (!buffer)
    {
        return NULL;
    }

    memset(buffer, 0, size + 1);
    fread(buffer, sizeof(char), size, file);

    fclose(file);

    return buffer;
}

void print_tree(Node* root, size_t depth)
{
    if (!root)
    {
        return;
    }

    for (size_t i = 0; i < depth; i++)
    {
        printf("  ");
    }

    printf("%s\n", root->string.str);

    print_tree(root->firstChild, depth + 1);
    print_tree(root->nextSibling, depth);
}

int main(int argc, char** argv)
{
    if (argc != 2)
    {
        printf("usage: %s <file>\n", argv[0]);
        return -1;
    }

    const char* path = argv[1];

    char* contents = read_file(path);
    if (!contents)
    {
        printf("failed to read file: %s\n", path);
        return 0;
    }

    Token* tokens = tokenize(contents);

    Token* list = tokens;
    while (list)
    {
        printf("token: %s\n", list->string.str);
        list = list->next;
    }

    Node* tree = parse(tokens);
    print_tree(tree, 0);

    generate(tree, "out.asm");

    system("nasm -f elf64 out.asm -o out.o");
    system("gcc out.o -o out.exe");

    // system("rm -f out.asm");
    system("rm -f out.o");

    return 0;
}