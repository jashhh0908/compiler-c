#ifndef CODEGEN_H
#define CODEGEN_H

#include "chunk.h"
#include "ast.h"

void compile(ASTNode* root, Chunk *chunk);
void debug_compile(ASTNode *root);

#endif
