#ifndef EVALUATOR_H
#define EVALUATOR_H
#include "syntax/ast.h"
#include "runtime/value.h"

typedef enum {
    EXEC_NORMAL,
    EXEC_BREAK,
} ExecutionResult;

void evaluate_program(ASTNode *program_root);

#endif