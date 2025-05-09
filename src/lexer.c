#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "lexer.h"

static const char* src;
static size_t pos = 0;

void init_lexer(const char* source_code) {
    src = source_code;
    pos = 0;
}

char peek() {
    return src[pos];
}

void advance() {
    pos++;
}

int isalpha_or_underscore(char c) {
    return isalpha(c) || c == '_';
}

int isalnum_or_underscore(char c) {
    return isalnum(c) || c == '_';
}

Token get_next_token() {
    // Saltar espacios en blanco
    while (isspace(peek())) {
        advance();
    }

    char current = peek();
    
    // Fin de archivo
    if (current == '\0') {
        Token token = { TOKEN_EOF, strdup("EOF") };
        return token;
    }
    
    // Comentarios
    if (current == '#') {
        advance(); // Consumir el '#'
        size_t start = pos;
        while (peek() != '\n' && peek() != '\0') {
            advance();
        }
        size_t length = pos - start;
        char* text = malloc(length + 1);
        strncpy(text, &src[start], length);
        text[length] = '\0';
        Token token = { TOKEN_COMMENT, text };
        return token;
    }
    
    // Identificadores y palabras clave
    if (isalpha_or_underscore(current)) {
        size_t start = pos;
        while (isalnum_or_underscore(peek())) {
            advance();
        }
        size_t length = pos - start;
        char* text = malloc(length + 1);
        strncpy(text, &src[start], length);
        text[length] = '\0';
        
        TokenType type = TOKEN_IDENTIFIER;
        // Verificar palabras clave
        if (strcmp(text, "echo") == 0) {
            type = TOKEN_ECHO;
        } else if (strcmp(text, "let") == 0) {
            type = TOKEN_LET;
        }
        
        Token token = { type, text };
        return token;
    }
    
    // Números
    if (isdigit(current)) {
        size_t start = pos;
        while (isdigit(peek())) {
            advance();
        }
        size_t length = pos - start;
        char* text = malloc(length + 1);
        strncpy(text, &src[start], length);
        text[length] = '\0';
        Token token = { TOKEN_NUMBER, text };
        return token;
    }
    
    // Operadores y símbolos
    advance(); // Consumir el carácter actual
    char* text = malloc(2);
    text[0] = current;
    text[1] = '\0';
    
    Token token;
    switch (current) {
        case '=': token.type = TOKEN_ASSIGN; break;
        case '+': token.type = TOKEN_PLUS; break;
        case '-': token.type = TOKEN_MINUS; break;
        case '*': token.type = TOKEN_STAR; break;
        case '/': token.type = TOKEN_SLASH; break;
        case ';': token.type = TOKEN_SEMICOLON; break;
        default: token.type = TOKEN_UNKNOWN; break;
    }
    
    token.text = text;
    return token;
}

void free_token(Token token) {
    free(token.text);
}