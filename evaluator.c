#include<stdio.h>
#include<stdlib.h>
#include "ast.h"
#include "evaluator.h"
#include "symbol_table.h"

static int evaluate_expression(ASTNode* exp, SymbolTable* table) {
    switch(exp->type) {
        case AST_NUMBER: {
            ASTNumber *num = (ASTNumber*)exp;
            return num->value;
        }

        case AST_IDENTIFIER: {
            ASTIdentifier *id = (ASTIdentifier*)exp;
            return symbolTable_get(table, id->name);
        }

        case AST_BINARYEXP: {
            ASTBinaryExp *bexp = (ASTBinaryExp*)exp;
            int left = evaluate_expression(bexp->left, table);
            int right = evaluate_expression(bexp->right, table);

            switch(bexp->op) {
                case '+': return left + right;
                case '-': return left - right;
                case '*': return left * right;
                case '/': 
                    if(right == 0) {
                        printf("Error: Division by zero is not possible\n");
                        exit(1);
                    }
                    return left / right;
                default: printf("Error: Unknown operator\n"); exit(1);
            }
        }

        default: printf("Error: Unknown Node Type for evaluation\n"); exit(1);
    }
}

static void evaluate_statement(ASTNode* smt, SymbolTable* table) {
    switch(smt->type) {
        case AST_ASSIGNMENT: {
            ASTAssignment *assign = (ASTAssignment*)smt;
            int value = evaluate_expression(assign->exp, table);
            symbolTable_set(table, assign->name, value);
            break;            
        }

        case AST_PRINT: {
            ASTPrint *print = (ASTPrint*)smt;
            if(print->exp->type == AST_STRING) {
                ASTString *s = (ASTString*)print->exp; //print->exp holds the actual string data
                printf("%s\n", s->str);
            } else {
                int x = evaluate_expression(print->exp, table);
                printf("%d\n", x);
            }
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
