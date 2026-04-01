#include <stdlib.h>
#include <stdio.h>
#include "chunk.h"
#include "ast.h"
#include "symbol_table.h"

char *opCodeName(Instruction code) {
    switch(code.opcode) {
        case OP_CONST: return "OP_CONST";
        case OP_ADD: return "OP_ADD";
        case OP_SUB: return "OP_SUB";
        case OP_DIV: return "OP_DIV";
        case OP_MUL: return "OP_MUL";
        case OP_LT: return "OP_LT";
        case OP_GT: return "OP_GT";
        case OP_GTE: return "OP_GTE";
        case OP_LTE: return "OP_LTE";
        case OP_EQ: return "OP_EQ";
        case OP_NEQ: return "OP_NEQ";
        case OP_LOAD: return "OP_LOAD";
        case OP_STORE: return "OP_STORE";
        case OP_POP: return "OP_POP";    
        case OP_JUMP: return "OP_JUMP";
        case OP_JUMP_IF_FALSE: return "OP_JUMP_IF_FALSE";
        case OP_PRINT: return "OP_PRINT";
        case OP_HALT: return "OP_HALT";
        default: printf("Code yet to put"); exit(1);
    }
}

int emitJump(Chunk *chunk, OpCode opcode) {
    return emitInstruction(chunk,opcode, -1);
}

void jumpOffset(Chunk *chunk, int jumpIndex) {
    int offset = chunk->count - jumpIndex - 1;
    chunk->code[jumpIndex].operand = offset;
}

int emitLoop(Chunk *chunk, OpCode opcode, int loopStart) {
    int offset = chunk->count - loopStart + 1;
    return emitInstruction(chunk, opcode, -offset);
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
    
    case AST_BOOL: {
        ASTBool *boolNode = (ASTBool*)node;

        Value v;
        v.type = VALUE_BOOL;
        v.val = boolNode->bool_value;

        int index = addConstant(v, chunk);
        emitInstruction(chunk, OP_CONST, index);
        break;
    }

    case AST_STRING: {
        ASTString *stringNode = (ASTString*)node;

        Value v;
        v.type = VALUE_STRING;
        v.str = stringNode->str;

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
        switch(bexp->op) {
            case '+': emitInstruction(chunk, OP_ADD, 0); break;
            case '-': emitInstruction(chunk, OP_SUB, 0); break;
            case '*': emitInstruction(chunk, OP_MUL, 0); break;
            case '/': emitInstruction(chunk, OP_DIV, 0); break;
            case '>': emitInstruction(chunk, OP_GT, 0); break;
            case '<': emitInstruction(chunk, OP_LT, 0); break;
            case 'g': emitInstruction(chunk, OP_GTE, 0); break;
            case 'l': emitInstruction(chunk, OP_LTE, 0); break;
            case '=': emitInstruction(chunk, OP_EQ, 0); break;
            case '!': emitInstruction(chunk, OP_NEQ, 0); break;
            default: printf("Compiling Error: Unknown binary operation '%c' found\n", bexp->op); exit(1); 
        }
        break;
    }
    
    case AST_IF: {
        ASTIf *ifNode = (ASTIf*)node;
        compileNode(ifNode->condition, chunk, table);
        int jumpIfFalse = emitJump(chunk, OP_JUMP_IF_FALSE);
        for(int i = 0; i < ifNode->if_stmt_count; i++){
            compileNode(ifNode->if_statements[i], chunk, table);
        }
        int jumpToEnd = emitJump(chunk, OP_JUMP);
        jumpOffset(chunk, jumpIfFalse);
        for(int i = 0; i < ifNode->else_stmt_count; i++){
            compileNode(ifNode->else_statements[i], chunk, table);
        }
        jumpOffset(chunk, jumpToEnd);
        break;
    }

    case AST_WHILE: {
        ASTWhile *loop = (ASTWhile*)node;
        int loopStart = chunk->count;
        compileNode(loop->condition, chunk, table);
        int jumpIfFalse = emitJump(chunk, OP_JUMP_IF_FALSE);
        for(int i = 0; i < loop->while_stmt_count; i++) {
            compileNode(loop->while_stmts[i], chunk, table);
        }
        int jumpBack = emitLoop(chunk, OP_JUMP, loopStart);
        jumpOffset(chunk, jumpIfFalse);
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
    default: printf("Unknown node type encountered in codegen\n: %d", node->type); exit(1);
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