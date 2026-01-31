#include<stdio.h>
#include "lexer.h"
#include "parser.h"
#include "ast.h"
#include<stdlib.h>

ASTNode *parse_program();
ASTNode *parse_statement();
ASTNode *parse_assignment();
ASTNode *parse_print_smt();
ASTNode *parse_if_smt();
ASTNode *parse_while_smt();
ASTNode *parse_break();
ASTNode *parse_expression();
ASTNode *parse_comparison();
ASTNode *parse_term();
ASTNode *parse_factor();
ASTNode *parse_logical_AND();
ASTNode *parse_logical_OR();


Token current_token;
static const char *token_name(TokenType type) {
    switch (type) {
        case TOKEN_EOF: return "EOF";
        case TOKEN_IDENTIFIER: return "IDENTIFIER";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_PRINT: return "PRINT";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_STRING: return "STRING";
        case TOKEN_EQ: return "==";
        case TOKEN_NEQ: return "!=";
        case TOKEN_AND: return "&&";
        case TOKEN_OR: return "||";
        case TOKEN_ASSIGN: return "ASSIGN";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_STAR: return "STAR";
        case TOKEN_SLASH: return "SLASH";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        case TOKEN_TRUE: return "TRUE";
        case TOKEN_FALSE: return "FALSE";
        case TOKEN_WHILE: return "WHILE";   
        case TOKEN_BREAK: return "BREAK";
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
        free(name); 
        return node;
    } else if(current_token.type == TOKEN_NUMBER){
        char *lex = current_token.lexeme;
        int value = convert_lexeme(lex);
        advance();
        free(lex);
        return make_number(value);
    } else if(current_token.type == TOKEN_LPAREN) {
        advance();
        ASTNode* exp = parse_logical_OR();
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

ASTNode *parse_logical_AND() {
    ASTNode *left = parse_comparison();
    while(current_token.type == TOKEN_AND) {
        advance();
        ASTNode *right = parse_comparison();
        left = make_binaryexp('&', left, right);
    }
    return left;
}

ASTNode *parse_logical_OR() {
    ASTNode *left = parse_logical_AND();
    while(current_token.type == TOKEN_OR) {
        advance();
        ASTNode *right = parse_logical_AND();
        left = make_binaryexp('|', left, right);
    }
    return left;
}

ASTNode* parse_print_smt() {
    char *print_lexeme = current_token.lexeme;
    consume(TOKEN_PRINT);
    free(print_lexeme);
    
    ASTNode *exp = parse_logical_OR();
    consume(TOKEN_SEMICOLON);

    return make_print_smt(exp);
}

ASTNode *parse_while_smt() {
    char *while_lexeme = current_token.lexeme;
    consume(TOKEN_WHILE);
    free(while_lexeme);

    consume(TOKEN_LPAREN);
    ASTNode *condition = parse_logical_OR();
    consume(TOKEN_RPAREN);
    consume(TOKEN_LBRACE);

    ASTNode *node = make_while(condition);
    ASTWhile *while_node = (ASTWhile*)node;
    while(current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_EOF) {
        ASTNode *statement = parse_statement();
        while_node->while_stmts = realloc(while_node->while_stmts, sizeof(ASTNode*) * (while_node->while_stmt_count + 1));
        while_node->while_stmts[while_node->while_stmt_count] = statement;
        while_node->while_stmt_count++; 
    }

    if(current_token.type == TOKEN_EOF) {
        printf("Unterminated while block");
        exit(1);
    }

    consume(TOKEN_RBRACE);
    return (ASTNode*)node;
}

ASTNode *parse_break() {
    consume(TOKEN_BREAK);
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_BREAK;
    consume(TOKEN_SEMICOLON);
    return node;
}

ASTNode* parse_if_smt() {
    char *if_lexeme = current_token.lexeme;
    consume(TOKEN_IF);
    free(if_lexeme);

    consume(TOKEN_LPAREN);
    ASTNode* condition = parse_logical_OR();
    consume(TOKEN_RPAREN);
    consume(TOKEN_LBRACE);

    ASTNode *node = make_if(condition);
    ASTIf *if_node = (ASTIf*)node;
    while(current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_EOF) {
        ASTNode* statement = parse_statement();
        if_node->if_statements = realloc(if_node->if_statements, sizeof(ASTNode*) * (if_node->if_stmt_count + 1));
        if_node->if_statements[if_node->if_stmt_count] = statement;
        if_node->if_stmt_count++;
    }
    consume(TOKEN_RBRACE);

    if(current_token.type == TOKEN_ELSE) {
        consume(TOKEN_ELSE);
        consume(TOKEN_LBRACE);
        
        while(current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_EOF) {
            ASTNode* statement = parse_statement();
            if_node->else_statements = realloc(if_node->else_statements, sizeof(ASTNode*) * (if_node->else_stmt_count + 1));
            if_node->else_statements[if_node->else_stmt_count] = statement;
            if_node->else_stmt_count++;
        }
        consume(TOKEN_RBRACE);
    }
    return (ASTNode*)node;
}

ASTNode* parse_assignment() {
    char *name = current_token.lexeme;
    consume(TOKEN_IDENTIFIER);
    consume(TOKEN_ASSIGN);
    
    ASTNode* exp = parse_logical_OR();
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
    } else if(current_token.type == TOKEN_WHILE) {
        return parse_while_smt();
    } else if(current_token.type == TOKEN_BREAK) {
        return parse_break();
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
            if(_if->else_stmt_count > 0) {
                for(int i = 0; i < level; i++) {
                   printf(" ");
                }
                printf("ELSE:\n");
                for(int i = 0; i < _if->else_stmt_count; i++) {
                    print_ast(_if->else_statements[i], level + 2);
                }
            }
            break;          
        }

        case AST_WHILE: {
            ASTWhile *_while = (ASTWhile*) node;
            printf("WHILE (Statements = %d)\n", _while->while_stmt_count);
            printf("CONDITION:\n");
            print_ast(_while->condition, level + 1);
            printf("BODY:\n"); {
                for(int i = 0; i < _while->while_stmt_count; i++) {
                    print_ast(_while->while_stmts[i], level + 1);
                }
            }
            break;
        }

        case AST_BREAK: printf("BREAK\n"); break;
         
        default: printf("Unknown Node\n");
    }
}