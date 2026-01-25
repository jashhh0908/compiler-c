#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"

int main(void) {
    const char *test = "x = 10 + 2 * 3; print x;";

    init_lexer(test);
    parse_program();
    printf("Parsed successfully!");
    return 0;
}
