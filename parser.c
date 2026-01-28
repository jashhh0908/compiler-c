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
ASTNode* parse_comparison();
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
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_FALSE: return "FALSE";
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
        ASTNode *node =  make_identifier(name);
        free(name); //
        return node;
    } else if(current_token.type == TOKEN_NUMBER){
        char *lex = current_token.lexeme;
        int value = convert_lexeme(lex);
        advance();
        free(lex);
        return make_number(value);
    } else if(current_token.type == TOKEN_LPAREN) {
        advance();
        ASTNode* exp = parse_comparison();
        consume(TOKEN_RPAREN);
        return exp;
    } else if (current_token.type == TOKEN_STRING) {
        ASTNode* str = make_string(current_token.lexeme);
        advance();
        return str;
    } else if(current_token.type == TOKEN_TRUE) {
        advance();
        return make_bool(1);
    } else if(current_token.type == TOKEN_FALSE) {
        advance();
        return make_bool(0);
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

ASTNode* parse_comparison() {
    ASTNode *left = parse_expression();
    while(current_token.type == TOKEN_EQ || current_token.type == TOKEN_NEQ) {
        char op;
        if(current_token.type == TOKEN_EQ)
            op = '=';
        else if(current_token.type == TOKEN_NEQ)
            op = '!';
        
        advance();
        ASTNode *right = parse_expression();
        left = make_binaryexp(op, left, right);
    }
    return left;
}

ASTNode* parse_print_smt() {
    char *print_lexeme = current_token.lexeme;
    consume(TOKEN_PRINT);
    free(print_lexeme);
    
    ASTNode *exp = parse_comparison();
    consume(TOKEN_SEMICOLON);

    return make_print_smt(exp);
}

ASTNode* parse_if_smt() {
    char *if_lexeme = current_token.lexeme;
    consume(TOKEN_IF);
    free(if_lexeme);

    consume(TOKEN_LPAREN);
    ASTNode* cond = parse_comparison();
    consume(TOKEN_RPAREN);
    consume(TOKEN_LBRACE);

    ASTIf *node = malloc(sizeof(ASTIf));
    node->type = AST_IF;
    node->condition = cond;
    node->if_statements = NULL;
    node->if_stmt_count = 0;
    while(current_token.type != TOKEN_RBRACE) {
        ASTNode* statement = parse_statement();
        node->if_statements = realloc(node->if_statements, sizeof(ASTNode*) * (node->if_stmt_count + 1));
        node->if_statements[node->if_stmt_count] = statement;
        node->if_stmt_count++;
    }
    consume(TOKEN_RBRACE);
    return (ASTNode*)node;
}

ASTNode* parse_assignment() {
    char *name = current_token.lexeme;
    consume(TOKEN_IDENTIFIER);
    consume(TOKEN_ASSIGN);
    
    ASTNode* exp = parse_comparison();
    consume(TOKEN_SEMICOLON);
    
    ASTNode *node = make_assignment(name, exp);
    free(name);
    return node;
}

ASTNode* parse_statement() {
    if(current_token.type == TOKEN_PRINT) {
        return parse_print_smt();    
    } else if(current_token.type == TOKEN_IDENTIFIER) {
        return parse_assignment();
    } else if(current_token.type == TOKEN_IF) {
        return parse_if_smt();
    } else {
        syntax_error(current_token.type, "statement (print or assignment or if)");
        return NULL;
    }
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
        
        case AST_BOOL: {
            ASTBool *b = (ASTBool*)node;
            printf("BOOL VALUE: %d\n", b->bool_value);
            break;
        }

        case AST_IF: {
            ASTIf *_if = (ASTIf*)node;  
            printf("IF (Statements = %d)\n", _if->if_stmt_count);
            printf("CONDITION:\n");
            print_ast(_if->condition, level + 1);
            printf("BODY:\n");
            for(int i = 0; i < _if->if_stmt_count; i++) {
                print_ast(_if->if_statements[i], level + 1);
            }
            break;          
        }
        default: printf("Unknown Node\n");
    }
}