#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_EOF,

    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,

    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,

    TOKEN_PRINT,

    TOKEN_SEMICOLON,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
} TokenType;

typedef struct {
    TokenType type;
    char *lexeme;
} Token;

void init_lexer(const char *source);
Token get_token(void);
int convert_lexeme(const char *str);
#endif