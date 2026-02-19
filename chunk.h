#ifndef CHUNK_H
#define CHUNK_H
#include "bytecode.h"
#include "value.h"
typedef struct {
    OpCode opcode;
    int operand;
} Instruction;

typedef struct {
    Value *values;
    int count;
    int capacity;
} ConstantPool;


typedef struct {
    Instruction *code;
    int count;
    ConstantPool constants;
} Chunk;

void initChunk(Chunk *chunk);
int addConstant(Value v, Chunk *chunk);
void emitInstruction(Chunk *chunk, OpCode opcode, int operand);
#endif