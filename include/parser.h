#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"

// Tipos de nodos AST
typedef enum {
    NODE_PROGRAM,         // Programa completo
    NODE_LET_STATEMENT,   // Declaración let
    NODE_ECHO_STATEMENT,  // Declaración echo
    NODE_EXPRESSION,      // Expresión
    NODE_BINARY_EXPR,     // Expresión binaria (ej: x + y)
    NODE_IDENTIFIER,      // Identificador
    NODE_NUMBER,          // Número literal
    NODE_STRING           // Cadena literal
} NodeType;

// Estructura base para cualquier nodo AST
typedef struct ASTNode {
    NodeType type;        // Tipo de nodo
} ASTNode;

// Estructura para representar una expresión binaria (ej: x + y)
typedef struct {
    ASTNode base;
    ASTNode* left;        // Operando izquierdo
    TokenType operator;   // Operador (+, -, *, /)
    ASTNode* right;       // Operando derecho
} BinaryExprNode;

// Estructura para representar un identificador
typedef struct {
    ASTNode base;
    char* name;           // Nombre del identificador
} IdentifierNode;

// Estructura para representar un número literal
typedef struct {
    ASTNode base;
    int value;            // Valor numérico
} NumberNode;

// Estructura para una declaración let (ej: let x = 5;)
typedef struct {
    ASTNode base;
    IdentifierNode* identifier;  // Nombre de la variable
    ASTNode* value;              // Valor asignado
} LetStatementNode;

// Estructura para una declaración echo (ej: echo x + y;)
typedef struct {
    ASTNode base;
    ASTNode* expression;   // Expresión a mostrar
} EchoStatementNode;

// Estructura para el programa completo
typedef struct {
    ASTNode base;
    ASTNode** statements;  // Array de declaraciones
    int count;             // Número de declaraciones
    int capacity;          // Capacidad del array
} ProgramNode;


typedef struct {
    ASTNode base;
    char* value;
} StringNode;

// Funciones del parser
ProgramNode* parse(const char* source_code);
void free_ast(ASTNode* node);
void print_ast(ASTNode* node, int indent);

#endif