#include <stdlib.h>
#include <stdio.h>
#include "chunk.h"
#include "ast.h"
#include "symbol_table.h"

char *opCodeName(Instruction code) {
    switch(code.opcode) {
        case OP_CONST: return "OP_CONST";
        case OP_ADD: return "OP_ADD";
        case OP_PRINT: return "OP_PRINT";
        case OP_HALT: return "OP_HALT";
        default: printf("Code yet to put"); exit(1);
    }
}
void compileNode(ASTNode *node, Chunk *chunk, SymbolTable *table) {
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
    
    case AST_ASSIGNMENT: {
        ASTAssignment *assign = (ASTAssignment*)node;
        compileNode(assign->exp, chunk, table);
        int index = symbolTable_getIndex(table, assign->name);
        emitInstruction(chunk, OP_STORE, index);
        break; 
    }

    case AST_IDENTIFIER: {
        ASTIdentifier *idn = (ASTIdentifier*)node;
        int index = symbolTable_find(table, idn->name);
        if(index == -1) {
            printf("Error: Undefined Variable %s\n", idn->name);
            exit(1);
        }
        emitInstruction(chunk, OP_LOAD, index);
        break;
    }

    case AST_BINARYEXP: {
        ASTBinaryExp *bexp = (ASTBinaryExp*)node;
        compileNode(bexp->left, chunk, table);
        compileNode(bexp->right, chunk, table);
        if(bexp->op == '+') {
            emitInstruction(chunk, OP_ADD, 0);
        }
        if(bexp->op == '-') {
            emitInstruction(chunk, OP_SUB, 0);
        }
        if(bexp->op == '*') {
            emitInstruction(chunk, OP_MUL, 0);
        }
        if(bexp->op == '/') {
            emitInstruction(chunk, OP_DIV, 0);
        }
        break;
    }
    
    case AST_PRINT: {
        ASTPrint *print = (ASTPrint*)node;
        compileNode(print->exp, chunk, table);
        emitInstruction(chunk, OP_PRINT, 0);
        break;
    }

    case AST_PROGRAM: {
        ASTProgram *root = (ASTProgram*)node;
        for(int i = 0; i < root->smt_count; i++) {
            compileNode(root->statements[i], chunk, table);
        }
        break;
    }
    default: printf("Unknown node type encountered in codegen\n"); exit(1);
    }
}

//main compilation
void compile(ASTNode *root, Chunk *chunk) {
    SymbolTable *table = symbolTable_create();
    initChunk(chunk);
    compileNode(root, chunk, table);
    emitInstruction(chunk, OP_HALT, 0);
}

//to debug
void debug_compile(ASTNode *root) {
    Chunk chunk;
    SymbolTable *table = symbolTable_create();
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