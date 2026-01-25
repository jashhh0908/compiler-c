#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
int main(void) {
    const char *test = "x = 10 + 2 * 3; print x;";

    init_lexer(test);
    ASTNode* success = parse_program();
    if(success != NULL)
        printf("Parsed successfully!");
    return 0;
}
