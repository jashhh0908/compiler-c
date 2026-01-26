#ifndef AST_H
#define AST_H

typedef enum {
    AST_PROGRAM,
    AST_ASSIGNMENT,
    AST_PRINT,
    AST_BINARYEXP,
    AST_IDENTIFIER,
    AST_NUMBER,    
} ASTNodeType;

typedef struct {
    ASTNodeType type;
} ASTNode;

typedef struct {
    ASTNodeType type;
    ASTNode **statements;
    int smt_count;
} ASTProgram;

typedef struct {
    ASTNodeType type;
    char *name;
    ASTNode* exp;
} ASTAssignment;

typedef struct {
    ASTNodeType type;
    ASTNode *exp;
} ASTPrint; 

typedef struct {
    ASTNodeType type;
    char op;
    ASTNode *left;
    ASTNode *right;
} ASTBinaryExp;

typedef struct {
    ASTNodeType type;
    char *name;
} ASTIdentifier;

typedef struct {
    ASTNodeType type;
    int value;
} ASTNumber;

ASTNode* make_number(int x);
ASTNode* make_identifier(char *name);
ASTNode* make_binaryexp(char op, ASTNode* left, ASTNode* right);
ASTNode* make_print_smt(ASTNode* exp);
ASTNode* make_assignment(char *name, ASTNode* exp);

//for testing purpose
void print_ast(ASTNode* node, int level);
#endif