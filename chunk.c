#include<stdlib.h>
#include "chunk.h"

void initChunk(Chunk *chunk) {
    chunk->code = NULL;
    chunk->count = 0;
    chunk->constants.values = NULL;
    chunk->constants.count = 0;
    chunk->constants.capacity = 0;
}

int addConstant(Value v, Chunk *chunk) {
    ConstantPool *pool = &chunk->constants;
    if (pool->count >= pool->capacity) {
        int newCapacity;
        if(pool->capacity == 0) 
            newCapacity = 8;
        else 
            newCapacity = pool->capacity * 2; 
        
        pool->values = realloc(pool->values, sizeof(Value) * newCapacity);
        pool->capacity = newCapacity;
    }
    pool->values[pool->count] = v;
    return pool->count++;
}

void emitInstruction(Chunk *chunk, OpCode opcode, int operand) {
    if(chunk->count == 0) {
        chunk->code = malloc(sizeof(Instruction) * 8);
    }
    chunk->code[chunk->count].opcode = opcode;
    chunk->code[chunk->count].operand = operand;
    chunk->count++;

}