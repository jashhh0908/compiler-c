#ifndef EVALUATOR_H
#define EVALUATOR_H
#include "ast.h"
#include "value.h"

typedef enum {
    EXEC_NORMAL,
    EXEC_BREAK,
} ExecutionResult;

void evaluate_program(ASTNode *program_root);

#endif