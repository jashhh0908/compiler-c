#include <stdlib.h>
#include <stdio.h>
#include "chunk.h"
#include "ast.h"

char *opCodeName(Instruction code) {
    switch(code.opcode) {
        case OP_CONST: return "OP_CONST";
        case OP_ADD: return "OP_ADD";
        case OP_PRINT: return "OP_PRINT";
        case OP_HALT: return "OP_HALT";
        default: printf("Code yet to put"); exit(1);
    }
}
void compileNode(ASTNode *node, Chunk *chunk) {
    switch (node->type) {
    case AST_NUMBER: {
        ASTNumber *num = (ASTNumber*)node;

        Value v;
        v.type = VALUE_INT;
        v.val = num->value;

        int index = addConstant(v, chunk);
        emitInstruction(chunk, OP_CONST, index);
        break;        
    }
    
    case AST_BINARYEXP: {
        ASTBinaryExp *bexp = (ASTBinaryExp*)node;
        compileNode(bexp->left, chunk);
        compileNode(bexp->right, chunk);
        if(bexp->op == '+') {
            emitInstruction(chunk, OP_ADD, 0);
        }
        break;
    }
    
    case AST_PRINT: {
        ASTPrint *print = (ASTPrint*)node;
        compileNode(print->exp, chunk);
        emitInstruction(chunk, OP_PRINT, 0);
        break;
    }

    case AST_PROGRAM: {
        ASTProgram *root = (ASTProgram*)node;
        for(int i = 0; i < root->smt_count; i++) {
            compileNode(root->statements[i], chunk);
        }
        break;
    }
    default: printf("Unknown node type encountered in codegen\n"); exit(1);
    }
}

//main compilation
void compile(ASTNode *root, Chunk *chunk) {
    initChunk(chunk);
    compileNode(root, chunk);
    emitInstruction(chunk, OP_HALT, 0);
}

//to debug
void debug_compile(ASTNode *root) {
    Chunk chunk;
    compile(root, &chunk);    
    printf("Constants:\n");
    for (int i = 0; i < chunk.constants.count; i++) {
        printf("  [%d] = %d\n", i, chunk.constants.values[i].val);
    }
    printf("Instructions:\n");
    for (int i = 0; i < chunk.count; i++) {
        printf("  %s operand = %d\n",
               opCodeName(chunk.code[i]),
               chunk.code[i].operand);
    }
}