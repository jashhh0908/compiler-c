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
        case VALUE_BOOL: return "boolean";
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
            
            if(bexp->op == '&' || bexp->op == '|') {
                Value left = evaluate_expression(bexp->left, table);
                if(left.type != VALUE_BOOL && left.type != VALUE_INT) {
                    printf("Runtime Error: Type mismatch (%s)", valueTypeName(left.type));
                    free_value(&left);
                    exit(1);
                }
                int result;
                if(bexp->op == '&') {
                    switch(left.type) {
                        case VALUE_INT: 
                            if(left.val == 0) {
                                free_value(&left);
                                return value_bool(0); 
                            } else {
                                Value right = evaluate_expression(bexp->right, table);
                                if(left.type != right.type) {
                                    printf("Runtime Error: Type mismatch (%s, %s)", valueTypeName(left.type), valueTypeName(right.type));
                                    free_value(&left);
                                    free_value(&right);
                                    exit(1);
                                }
                                result = (left.val && right.val); 
                                free_value(&right);
                                break;
                            }
                        case VALUE_BOOL: 
                            if(!left.bool_val) {
                                free_value(&left);
                                return value_bool(0);
                            } else {
                                Value right = evaluate_expression(bexp->right, table);
                                if(left.type != right.type) {
                                    printf("Runtime Error: Type mismatch (%s, %s)", valueTypeName(left.type), valueTypeName(right.type));
                                    free_value(&left);
                                    free_value(&right);
                                    exit(1);
                                }
                                result = (left.bool_val && right.bool_val); 
                                free_value(&right);
                                break;
                            }
                        case VALUE_STRING: printf("Runtime Error: Cannot evalute && on strings\n"); exit(1);
                        default: printf("Runtime Error: Unknown Value Type"); exit(1);
                    }
                } else if(bexp->op == '|') {
                    switch(left.type) {
                        case VALUE_INT: 
                            if(left.val != 0) {
                                free_value(&left);
                                return value_bool(1); 
                            } else {
                                Value right = evaluate_expression(bexp->right, table);
                                if(left.type != right.type) {
                                    printf("Runtime Error: Type mismatch (%s, %s)", valueTypeName(left.type), valueTypeName(right.type));
                                    free_value(&left);
                                    free_value(&right);
                                    exit(1);
                                }
                                result = (left.val || right.val); 
                                free_value(&right);
                                break;
                            }
                        case VALUE_BOOL: 
                            if(left.bool_val) {
                                free_value(&left);
                                return value_bool(1);
                            } else { 
                                Value right = evaluate_expression(bexp->right, table);
                                if(left.type != right.type) {
                                    printf("Runtime Error: Type mismatch (%s, %s)", valueTypeName(left.type), valueTypeName(right.type));
                                    free_value(&left);
                                    free_value(&right);
                                    exit(1);
                                }
                                result = (left.bool_val || right.bool_val); 
                                free_value(&right);
                                break;
                            }
                        case VALUE_STRING: printf("Runtime Error: Cannot evalute || on strings\n"); exit(1);
                        default: printf("Runtime Error: Unknown Value Type"); exit(1);
                    }
                }
                free_value(&left);
                return value_bool(result);
            }
            
            Value left = evaluate_expression(bexp->left, table);
            Value right = evaluate_expression(bexp->right, table);
            if(bexp->op == '=' || bexp->op == '!') {
                if(left.type != right.type) {
                    printf("Runtime Error: Type mismatch (%s, %s)", valueTypeName(left.type), valueTypeName(right.type));
                    free_value(&left);
                    free_value(&right);
                    exit(1);
                }
                int result;
                if(bexp->op == '=') {
                    switch(left.type) {
                        case VALUE_INT: result = (left.val == right.val); break; 
                        case VALUE_BOOL: result = (left.bool_val == right.bool_val); break;
                        case VALUE_STRING: result = (strcmp(left.str, right.str) == 0); break;
                        default: printf("Runtime Error: Unknown Value Type"); exit(1);
                    }
                }
                else if(bexp->op == '!') {
                    switch(left.type) {
                        case VALUE_INT: result = (left.val != right.val); break;
                        case VALUE_BOOL: result = (left.bool_val != right.bool_val); break;
                        case VALUE_STRING: result = (strcmp(left.str, right.str) != 0); break;
                        default: printf("Runtime Error: Unknown Value Type"); exit(1);
                    }
                }
                free_value(&left);
                free_value(&right);
                return value_bool(result);
            }
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

        case AST_BOOL: {
            ASTBool *b = (ASTBool*)exp;
            return value_bool(b->bool_value);
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
            } else if(x.type == VALUE_BOOL) {
                printf("%s\n", x.bool_val ? "true" : "false");
            }
            free_value(&x);
            break;
        }

        case AST_IF: {
            ASTIf *_if = (ASTIf*)smt;
            Value condition = evaluate_expression(_if->condition, table);
            if(condition.type != VALUE_BOOL) {
                printf("Runtime Error: condition expression must return a boolean value\n");
                free_value(&condition);
                exit(1);
            }

            if(condition.bool_val) {
                for(int i = 0; i < _if->if_stmt_count; i++) {
                    evaluate_statement(_if->if_statements[i], table);
                }
            } else if(_if->else_stmt_count > 0) {
                for(int i = 0; i < _if->else_stmt_count; i++) {
                    evaluate_statement(_if->else_statements[i], table);
                }
            }

            free_value(&condition);
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
