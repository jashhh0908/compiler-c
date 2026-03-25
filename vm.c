#include "vm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

int isTruthy(Value v) {
    switch(v.type) {
        case VALUE_BOOL: return v.bool_val; 
        case VALUE_INT: return v.val != 0;
        case VALUE_STRING: return v.str != NULL;
        default: return 0;
    }
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

            case OP_STORE: {
                Value v = pop(vm);
                vm->globals[instruction.operand] = v;
                break;
            }
            
            case OP_LOAD: {
                Value v = vm->globals[instruction.operand];
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

            case OP_LT: {
                Value b = pop(vm);
                Value a = pop(vm);

                if(a.type != VALUE_INT || b.type != VALUE_INT) {
                    printf("Runtime Error: '<' operator expects integers\n");
                    return;
                }

                int result = a.val < b.val;
                push(vm, value_bool(result));
                break;
            }

            case OP_GT: {
                Value b = pop(vm);
                Value a = pop(vm);

                if(a.type != VALUE_INT || b.type != VALUE_INT) {
                    printf("Runtime Error: '>' operator expects integers\n");
                    return;
                }
                int result = a.val > b.val;
                push(vm, value_bool(result));
                break;
            }

            case OP_LTE: {
                Value b = pop(vm);
                Value a = pop(vm);

                if(a.type != VALUE_INT || b.type != VALUE_INT) {
                    printf("Runtime Error: '<=' operator expects integers\n");
                    return;
                }

                int result = a.val <= b.val;
                push(vm, value_bool(result));
                break;
            }

            case OP_GTE: {
                Value b = pop(vm);
                Value a = pop(vm);

                if(a.type != VALUE_INT || b.type != VALUE_INT) {
                    printf("Runtime Error: '>=' operator expects integers\n");
                    return;
                }

                int result = a.val >= b.val;
                push(vm, value_bool(result));
                break;
            }

            case OP_EQ: {
                Value b = pop(vm);
                Value a = pop(vm);

                if(a.type != b.type) {
                    printf("Runtime error: Type mismatch for '==' comparison");
                    return;
                }

                int result = 0;
                switch (a.type) {
                    case VALUE_INT: {
                        result = (a.val == b.val);
                        break;
                    }
                    case VALUE_BOOL: {
                        result = (a.bool_val == b.bool_val);
                        break;
                    }
                    case VALUE_STRING: {
                        result = (strcmp(a.str, b.str) == 0);
                        break;

                    }
                    default: printf("Runtime Error: unknown value type\n"); exit(1);
                    break;
                }
                push(vm, value_bool(result));
                break;
            }

            case OP_NEQ: {
                Value b = pop(vm);
                Value a = pop(vm);

                if(a.type != b.type) {
                    printf("Runtime error: Type mismatch for '!=' comparison");
                    return;
                }

                int result = 0;
                switch (a.type) {
                    case VALUE_INT: {
                        result = (a.val != b.val);
                        break;
                    }
                    case VALUE_BOOL: {
                        result = (a.bool_val != b.bool_val);
                        break;
                    }
                    case VALUE_STRING: {
                        result = (strcmp(a.str, b.str) != 0);
                        break;

                    }
                    default: printf("Runtime Error: unknown value type\n"); exit(1);
                    break;
                }
                push(vm, value_bool(result));
                break;
            }

            case OP_JUMP_IF_FALSE: {
                Value cond = pop(vm);
                if(!isTruthy(cond)) {
                    vm->ip += instruction.operand;
                }
                break;
            }
            case OP_PRINT: {
                Value v = pop(vm);
                switch(v.type) {
                    case VALUE_INT: printf("%d\n", v.val); break;
                    case VALUE_BOOL: 
                        if(v.bool_val == 1) printf("true\n"); 
                        else if(v.bool_val == 0) printf("false\n");
                        break;
                    case VALUE_STRING: printf("%s\n", v.str);
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
