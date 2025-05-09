#ifndef LEXER_H
#define LEXER_H

typedef enum {
    TOKEN_LET,
    TOKEN_ECHO,      
    TOKEN_IDENTIFIER,
    TOKEN_NUMBER,
    TOKEN_ASSIGN,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_STAR,
    TOKEN_SLASH,
    TOKEN_SEMICOLON,
    TOKEN_COMMENT,
    TOKEN_EOF,
    TOKEN_UNKNOWN
} TokenType;

typedef struct {
    TokenType type;
    char* text;
} Token;

void init_lexer(const char* source_code);
Token get_next_token();
void free_token(Token token);

#endif