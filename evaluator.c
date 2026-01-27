#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "ast.h"
#include "evaluator.h"
#include "symbol_table.h"
#include "value.h"

char *valueTypeName(ValueType v) {
    switch(v) {
        case VALUE_INT: return "int";
        case VALUE_STRING: return "string";
        default: return "unknown";
    }
}
static Value evaluate_expression(ASTNode* exp, SymbolTable* table) {
    switch(exp->type) {
        case AST_NUMBER: {
            ASTNumber *num = (ASTNumber*)exp;
            return value_int(num->value);
        }

        case AST_IDENTIFIER: {
            ASTIdentifier *id = (ASTIdentifier*)exp;
            return symbolTable_get(table, id->name);
        }

        case AST_STRING: {
            ASTString *s = (ASTString*)exp;
            return value_string(s->str);
        }
        case AST_BINARYEXP: {
            ASTBinaryExp *bexp = (ASTBinaryExp*)exp;
            Value left = evaluate_expression(bexp->left, table);
            Value right = evaluate_expression(bexp->right, table);

            if(left.type == VALUE_INT && right.type == VALUE_INT) {
                int result;
                switch(bexp->op) {
                    case '+': result = left.val + right.val; break;
                    case '-': result = left.val - right.val; break;
                    case '*': result = left.val * right.val; break;
                    case '/': 
                        if(right.val == 0) {
                            printf("Error: Division by zero is not possible\n");
                            exit(1);
                        }
                        result = left.val / right.val;
                        break;
                    default: printf("Error: Unknown operator\n"); exit(1);
                }
                free_value(&left);
                free_value(&right);
                return value_int(result);
            }

            if(left.type == VALUE_STRING && right.type == VALUE_STRING && bexp->op == '+') {
                size_t length = strlen(left.str) + strlen(right.str) + 1;
                char *concat_str = malloc(length);
                strcpy(concat_str, left.str);
                strcat(concat_str, right.str);

                free_value(&left);
                free_value(&right);
                Value v = value_string(concat_str);
                free(concat_str);
                return v;
            }
            printf("Runtime Error: cannot apply operator (%c) to types (%s, %s)\n", bexp->op, valueTypeName(left.type), valueTypeName(right.type));
            free_value(&left);
            free_value(&right);
            exit(1);
        }
        default: printf("Error: Unknown Node Type for evaluation\n"); exit(1);
    }
}

static void evaluate_statement(ASTNode* smt, SymbolTable* table) {
    switch(smt->type) {
        case AST_ASSIGNMENT: {
            ASTAssignment *assign = (ASTAssignment*)smt;
            Value value = evaluate_expression(assign->exp, table);
            symbolTable_set(table, assign->name, value);
            free_value(&value);
            break;            
        }

        case AST_PRINT: {
            ASTPrint *print = (ASTPrint*)smt;
            Value x = evaluate_expression(print->exp, table);
            if(x.type == VALUE_STRING) {
                printf("%s\n", x.str);
            } else if(x.type == VALUE_INT) {
                printf("%d\n", x.val);
            }
            free_value(&x);
            break;
        }
        default: printf("Unknown Node\n"); exit(1);
    }
}

void evaluate_program(ASTNode *program_root) {
    SymbolTable *table = symbolTable_create();
    int i;
    ASTProgram *program = (ASTProgram*)program_root;
    for(i = 0; i < program->smt_count; i++) {
        evaluate_statement(program->statements[i], table);
    }

    symbolTable_free(table);
}
