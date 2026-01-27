#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include "evaluator.h"

char *read_file() {
    FILE* file = fopen("tests/string_concat.txt", "rb");
    if(!file) {
        printf("Could not open\n");
        return NULL;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char *buffer = malloc(size + 1);
    fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);

    return buffer;
}
int main(void) {
    const char *test = read_file();

    init_lexer(test);
    ASTNode* success = parse_program();
    if(success != NULL) {
        printf("Parsed successfully!\n");
        evaluate_program(success);
    }
    return 0;
}
