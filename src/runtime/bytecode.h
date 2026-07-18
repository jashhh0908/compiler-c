#ifndef BYTECODE_H
#define BYTECODE_H

typedef enum {
    OP_CONST,          
    OP_POP,            

    OP_LOAD,           
    OP_STORE,          

    OP_ADD,            
    OP_SUB,
    OP_MUL,
    OP_DIV,

    OP_EQ,
    OP_NEQ,
    OP_LT,
    OP_GT,
    OP_LTE,
    OP_GTE,

    OP_NOT,

    OP_JUMP,            
    OP_JUMP_IF_FALSE,   

    OP_PRINT,

    OP_HALT
} OpCode;

#endif
