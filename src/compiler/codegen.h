#ifndef CODEGEN_H
#define CODEGEN_H

#include "runtime/chunk.h"
#include "syntax/ast.h"

void compile(ASTNode* root, Chunk *chunk);
void debug_compile(ASTNode *root);

#endif
