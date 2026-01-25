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