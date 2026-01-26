#include<stdio.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include<stdlib.h>

ASTNode* parse_program();
ASTNode* parse_statement();
ASTNode* parse_assignment();
ASTNode* parse_print_smt();
ASTNode* parse_expression();
ASTNode* parse_term();
ASTNode* parse_factor();

Token current_token;
static const char *token_name(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_PRINT: return "PRINT";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_STAR: return "STAR";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        default: return "UNKNOWN";
    }
}

void advance() {
    current_token = get_token();
}

void syntax_error(TokenType current, const char* expected) {
    printf("Syntax error: Expected %s, but found %s", expected, token_name(current));
    exit(1);
}

void consume(TokenType expected) {
    if(current_token.type == expected) {
        advance();
    } else {
        syntax_error(current_token.type, token_name(expected));
    }
}

ASTNode* parse_factor() {
    if(current_token.type == TOKEN_IDENTIFIER) {
        char *name = current_token.lexeme;
        advance();
        return make_identifier(name);

    } else if(current_token.type == TOKEN_NUMBER){
        int value = convert_lexeme(current_token.lexeme);
        int x = value;
        advance();
        return make_number(x);
    } else if(current_token.type == TOKEN_LPAREN) {
        advance();
        ASTNode* exp = parse_expression();
        consume(TOKEN_RPAREN);
        return exp;
    } else {
        syntax_error(current_token.type, "IDENTIFIER or NUMBER or '('");
        return NULL;
    }
}

ASTNode* parse_term() {
    ASTNode *left = parse_factor();
    while(current_token.type == TOKEN_STAR || current_token.type == TOKEN_SLASH) {
        char op;
        if(current_token.type == TOKEN_STAR)
            op = '*';
        else if(current_token.type == TOKEN_SLASH)
            op = '/';
        
        advance();
        ASTNode* right = parse_factor();
        left = make_binaryexp(op, left, right);
    }

    return left;
}

ASTNode* parse_expression() {
    ASTNode *left = parse_term();
    while(current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS) {
        char op;
        if(current_token.type == TOKEN_PLUS)
            op = '+';
        else if(current_token.type == TOKEN_MINUS)
            op = '-';
        
        advance();
        ASTNode* right = parse_term();
        left = make_binaryexp(op, left, right);
    }
    return left;
}

ASTNode* parse_statement() {
    if(current_token.type == TOKEN_PRINT) {
        return parse_print_smt();    
    } else if(current_token.type == TOKEN_IDENTIFIER) {
        return parse_assignment();
    } else {
        syntax_error(current_token.type, "statement (print or assignment)");
        return NULL;
    }
}

ASTNode* parse_print_smt() {
    consume(TOKEN_PRINT);
    if(current_token.type == TOKEN_STRING) {
        char *str = current_token.lexeme;
        consume(TOKEN_STRING);
        consume(TOKEN_SEMICOLON);
        ASTNode* s = make_string(str);
        return make_print_smt(s);
    }
    
    ASTNode *exp = parse_expression();
    consume(TOKEN_SEMICOLON);

    return make_print_smt(exp);
}

ASTNode* parse_assignment() {
    char *name = current_token.lexeme;
    consume(TOKEN_IDENTIFIER);
    consume(TOKEN_ASSIGN);
    ASTNode* exp = parse_expression();
    consume(TOKEN_SEMICOLON);

    return make_assignment(name, exp);
}

ASTNode* parse_program() {
    ASTProgram* node = malloc(sizeof(ASTProgram));
    node->type = AST_PROGRAM;
    node->statements = NULL;
    node->smt_count = 0;
    advance();
    while(current_token.type != TOKEN_EOF) {
        ASTNode* statement = parse_statement();
        node->statements = realloc(node->statements, sizeof(ASTNode*) * (node->smt_count + 1));
        node->statements[node->smt_count] = statement;
        node->smt_count++;
    }
    return (ASTNode*)node;
}

//for testing purpose
void print_ast (ASTNode *node, int level) {
    if(node == NULL) return;

    for(int i = 0; i < level; i++) {
        printf(" ");
    }

    switch(node->type) {
        case AST_PROGRAM: { 
            
            ASTProgram *p = (ASTProgram*)node;
            printf("Program (Statements = %d)\n", p->smt_count);
            
            for(int i = 0; i < p->smt_count; i++) {
                print_ast(p->statements[i], level + 1);
            }
            break;
        }

        case AST_ASSIGNMENT: {
            ASTAssignment *assign = (ASTAssignment*)node;
            printf("ASSIGN TO: %s\n", assign->name);
            print_ast(assign->exp, level + 1);
            break;
        }

        case AST_PRINT: {
            ASTPrint *p = (ASTPrint*)node;
            printf("PRINT\n");
            print_ast(p->exp, level + 1);
            break;
        }
        
        case AST_BINARYEXP: {
            ASTBinaryExp *bexp = (ASTBinaryExp*)node;
            printf("Operation: %c\n", bexp->op);
            print_ast(bexp->left, level + 1);
            print_ast(bexp->right, level + 1);
            break;
        }

        case AST_IDENTIFIER: {
            ASTIdentifier *id = (ASTIdentifier*)node;
            printf("VAR: %s\n", id->name);
            break;
        }

        case AST_NUMBER: {
            ASTNumber *num = (ASTNumber*)node;
            printf("Value: %d\n", num->value);
            break;
        }

        case AST_STRING: {
            ASTString *s = (ASTString*)node;
            printf("STRING: %s\n", s->str);
            break;
        }
        default: printf("Unknown Node\n");
    }
}