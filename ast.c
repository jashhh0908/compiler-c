#include<stdlib.h>
#include "ast.h"
#include<Stdio.h>
#include<string.h>
ASTNode* make_number(int x) {
    ASTNumber* node = malloc(sizeof(ASTNumber));

    node->type = AST_NUMBER;
    node->value = x;

    return (ASTNode*)node;
}

ASTNode* make_identifier(char *name) {
    ASTIdentifier* node = malloc(sizeof(ASTIdentifier));

    node->type = AST_IDENTIFIER;
    node->name = malloc(strlen(name) + 1);
    strcpy(node->name, name);

    return (ASTNode*)node;
}

ASTNode* make_binaryexp(char op, ASTNode* left, ASTNode *right) {
    ASTBinaryExp* node = malloc(sizeof(ASTBinaryExp));

    node->type = AST_BINARYEXP;
    node->op = op;
    node->left = left;
    node->right = right;

    return (ASTNode*)node;
}

ASTNode* make_print_smt(ASTNode* exp) {
    ASTPrint* node = malloc(sizeof(ASTPrint));

    node->type = AST_PRINT;
    node->exp = exp;

    return (ASTNode*)node;
}

ASTNode* make_assignment(char *name, ASTNode* exp) {
    ASTAssignment* node = malloc(sizeof(ASTAssignment));

    node->type = AST_ASSIGNMENT;
    node->name = malloc(strlen(name) + 1);
    strcpy(node->name, name);   
    node->exp = exp;

    return (ASTNode*)node;
}