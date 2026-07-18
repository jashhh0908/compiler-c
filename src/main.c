#include <stdio.h>
#include <stdlib.h>
#include "syntax/lexer.h"
#include "syntax/parser.h"
#include "syntax/ast.h"
#include "compiler/evaluator.h"
#include "compiler/codegen.h"
#include "runtime/vm.h"
char *read_file(const char* filename) {
    FILE* file = fopen(filename, "rb");
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
int main(int argc, char* argv[]) {
    if(argc < 2) {
        printf("Usage: compiler-c <filename>\n");
        return 1;
    }
    const char *test = read_file(argv[1]);
    if(!test) return 1;
    init_lexer(test);
    ASTNode* success = parse_program();
    if(success != NULL) {
        printf("Parsed successfully!\n");
        //print_ast(success, 0);
        //debug_compile(success);
        Chunk chunk;
        compile(success, &chunk);
        VM vm;
        initVM(&vm, &chunk);
        run(&vm);
    }
    return 0;
}
