#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
void initVM(VM *vm, Chunk *chunk) {
    vm->chunk = chunk;
    vm->ip = chunk->code;
    vm->top = vm->stack;
}

static void push(VM *vm, Value value) {
    *vm->top = value;
    vm->top++;
}

static Value pop(VM *vm) {
    if (vm->top == vm->stack) {
        printf("Runtime Error: Stack underflow\n");
        exit(1);
    }
    vm->top--;
    return *vm->top;
}

void run(VM *vm) {
    while(1) {
        Instruction instruction = *vm->ip;
        vm->ip++;

        switch(instruction.opcode) {
            case OP_CONST: {
                Value v = vm->chunk->constants.values[instruction.operand];
                push(vm, v);
                break;
            }

            case OP_ADD: {
                Value b = pop(vm);
                Value a = pop(vm);
                if (a.type != VALUE_INT || b.type != VALUE_INT) {
                    printf("Runtime Error: ADD expects integers\n");
                    return;
                }

                Value result;   
                result = value_int(a.val + b.val);
                push(vm, result);
                break;
            }

            case OP_SUB: {
                Value b = pop(vm);
                Value a = pop(vm);
                if (a.type != VALUE_INT || b.type != VALUE_INT) {
                    printf("Runtime Error: SUB expects integers\n");
                    return;
                }

                Value result;   
                result = value_int(a.val - b.val);
                push(vm, result);
                break;
            }

            case OP_MUL: {
                Value b = pop(vm);
                Value a = pop(vm);
                if (a.type != VALUE_INT || b.type != VALUE_INT) {
                    printf("Runtime Error: MUL expects integers\n");
                    return;
                }

                Value result;   
                result = value_int(a.val * b.val);
                push(vm, result);
                break;
            }

            case OP_DIV: {
                Value b = pop(vm);
                Value a = pop(vm);
                if (a.type != VALUE_INT || b.type != VALUE_INT) {
                    printf("Runtime Error: ADD expects integers\n");
                    return;
                }
                if(b.val == 0) {
                    printf("Runtime Error: DIV cannot be applied to divisor 0\n");
                    return;
                }
                Value result;   
                result = value_int(a.val / b.val);
                push(vm, result);
                break;
            }
            case OP_PRINT: {
                Value v = pop(vm);
                if(v.type == VALUE_INT) {
                    printf("%d\n", v.val);
                }
                break;
            }
            case OP_HALT: return;
            default:
                printf("Unknown opcode encountered.\n");
                return;

        }
    }
}
