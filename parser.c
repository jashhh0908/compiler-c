#include<stdio.h>
#include "lexer.h"
#include "parser.h"
#include<stdlib.h>

void parse_program();
void parse_statement();
void parse_assignment();
void parse_print_smt();
void parse_expression();
void parse_term();
void parse_factor();

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

void parse_factor() {
    if(current_token.type == TOKEN_IDENTIFIER || current_token.type == TOKEN_NUMBER) {
        advance();
    } else if(current_token.type == TOKEN_LPAREN) {
        advance();
        parse_expression();
        consume(TOKEN_RPAREN);
    } else {
        syntax_error(current_token.type, "INDENTIFIER OR NUMBER OR '('");
    }
}

void parse_term() {
    parse_factor();
    while(current_token.type == TOKEN_STAR || current_token.type == TOKEN_SLASH) {
        advance();
        parse_factor();
    }
}

void parse_expression() {
    parse_term();
    while(current_token.type == TOKEN_PLUS || current_token.type == TOKEN_MINUS) {
        advance();
        parse_term();
    }
}

void parse_statement() {
    if(current_token.type == TOKEN_PRINT) {
        parse_print_smt();    
    } else if(current_token.type == TOKEN_IDENTIFIER) {
        parse_assignment();
    } else {
        syntax_error(current_token.type, "statement (print or assignment)");
    }
}

void parse_print_smt() {
    consume(TOKEN_PRINT);
    parse_expression();
    consume(TOKEN_SEMICOLON);
}

void parse_assignment() {
    consume(TOKEN_IDENTIFIER);
    consume(TOKEN_ASSIGN);
    parse_expression();
    consume(TOKEN_SEMICOLON);
}

void parse_program() {
    advance();
    while(current_token.type != TOKEN_EOF) {
        parse_statement();
    }
}