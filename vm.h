#ifndef VM_H
#define VM_H

#include<stdint.h>
#include "chunk.h"
#define SIZE 256

typedef struct {
    Chunk *chunk;
    Instruction *ip; 
    Value stack[SIZE];
    Value *top;  
} VM;

void initVM(VM *vm, Chunk *chunk);
void run(VM *vm);
#endif