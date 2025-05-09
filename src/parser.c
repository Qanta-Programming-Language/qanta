#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

// Variables globales del parser
static Token current_token;
static Token peek_token;

// Funciones de utilidad
void next_token()
{
    current_token = peek_token;
    peek_token = get_next_token();
}

int expect_peek(TokenType type)
{
    if (peek_token.type == type)
    {
        next_token();
        return 1;
    }
    fprintf(stderr, "Error de sintaxis: se esperaba %d pero se encontró %d\n", type, peek_token.type);
    return 0;
}

// Funciones para crear nodos AST
IdentifierNode *new_identifier(char *name)
{
    IdentifierNode *node = malloc(sizeof(IdentifierNode));
    node->base.type = NODE_IDENTIFIER;
    node->name = strdup(name);
    return node;
}

NumberNode *new_number(int value)
{
    NumberNode *node = malloc(sizeof(NumberNode));
    node->base.type = NODE_NUMBER;
    node->value = value;
    return node;
}

BinaryExprNode *new_binary_expr(ASTNode *left, TokenType operator, ASTNode *right)
{
    BinaryExprNode *node = malloc(sizeof(BinaryExprNode));
    node->base.type = NODE_BINARY_EXPR;
    node->left = left;
    node->operator = operator;
    node->right = right;
    return node;
}

LetStatementNode *new_let_statement(IdentifierNode *identifier, ASTNode *value)
{
    LetStatementNode *node = malloc(sizeof(LetStatementNode));
    node->base.type = NODE_LET_STATEMENT;
    node->identifier = identifier;
    node->value = value;
    return node;
}

EchoStatementNode *new_echo_statement(ASTNode *expression)
{
    EchoStatementNode *node = malloc(sizeof(EchoStatementNode));
    node->base.type = NODE_ECHO_STATEMENT;
    node->expression = expression;
    return node;
}

ProgramNode *new_program()
{
    ProgramNode *program = malloc(sizeof(ProgramNode));
    program->base.type = NODE_PROGRAM;
    program->capacity = 10;
    program->count = 0;
    program->statements = malloc(sizeof(ASTNode *) * program->capacity);
    return program;
}

StringNode *new_string(const char *value)
{
    StringNode *node = malloc(sizeof(StringNode));
    node->base.type = NODE_STRING;
    node->value = strdup(value);
    return node;
}

void add_statement(ProgramNode *program, ASTNode *statement)
{
    if (program->count >= program->capacity)
    {
        program->capacity *= 2;
        program->statements = realloc(program->statements, sizeof(ASTNode *) * program->capacity);
    }
    program->statements[program->count++] = statement;
}

// Declaraciones de funciones del parser
ASTNode *parse_expression(int precedence);
ASTNode *parse_statement();
ASTNode *parse_let_statement();
ASTNode *parse_echo_statement();
ASTNode *parse_identifier();
ASTNode *parse_number();
ASTNode *parse_expression_statement();
ASTNode *parse_string();

// Mapeo de precedencia para operadores
int get_precedence(TokenType token_type)
{
    switch (token_type)
    {
    case TOKEN_PLUS:
    case TOKEN_MINUS:
        return 1;
    case TOKEN_STAR:
    case TOKEN_SLASH:
        return 2;
    default:
        return 0;
    }
}

// Parsear un identificador
ASTNode *parse_identifier()
{
    return (ASTNode *)new_identifier(current_token.text);
}

// Parsear un número
ASTNode *parse_number()
{
    NumberNode *node = new_number(atoi(current_token.text));
    return (ASTNode *)node;
}

// Parsear una expresión
ASTNode *parse_expression(int precedence)
{
    ASTNode *left = NULL;

    // Parsear el término izquierdo
    if (current_token.type == TOKEN_IDENTIFIER)
    {
        left = parse_identifier();
    }
    else if (current_token.type == TOKEN_NUMBER)
    {
        left = parse_number();
    }
    else if (current_token.type == TOKEN_STRING)
    {
        left = parse_string();
    }
    else
    {
        fprintf(stderr, "Error: expresión no válida\n");
        return NULL;
    }

    // Parsear operadores binarios si tienen mayor precedencia
    while (peek_token.type != TOKEN_SEMICOLON && precedence < get_precedence(peek_token.type))
    {
        TokenType operator = peek_token.type;
        next_token(); // Consumir el operador

        next_token(); // Avanzar al término derecho
        ASTNode *right = parse_expression(get_precedence(operator));

        left = (ASTNode *)new_binary_expr(left, operator, right);
    }

    return left;
}

// Parsear una declaración let
ASTNode *parse_let_statement()
{
    if (!expect_peek(TOKEN_IDENTIFIER))
    {
        return NULL;
    }

    IdentifierNode *identifier = new_identifier(current_token.text);

    if (!expect_peek(TOKEN_ASSIGN))
    {
        free(identifier);
        return NULL;
    }

    next_token(); // Avanzar al valor
    ASTNode *value = parse_expression(0);

    if (peek_token.type == TOKEN_SEMICOLON)
    {
        next_token(); // Consumir el punto y coma
    }

    return (ASTNode *)new_let_statement(identifier, value);
}

// Parsear una declaración echo
ASTNode *parse_echo_statement()
{
    next_token(); // Avanzar a la expresión
    ASTNode *expr = parse_expression(0);

    if (peek_token.type == TOKEN_SEMICOLON)
    {
        next_token(); // Consumir el punto y coma
    }

    return (ASTNode *)new_echo_statement(expr);
}

// Parsear una declaración
ASTNode *parse_statement()
{
    switch (current_token.type)
    {
    case TOKEN_LET:
        return parse_let_statement();
    case TOKEN_ECHO:
        return parse_echo_statement();
    default:
        fprintf(stderr, "Error: declaración no reconocida: %d\n", current_token.type);
        return NULL;
    }
}

ASTNode *parse_string()
{
    StringNode *node = new_string(current_token.text);
    return (ASTNode *)node;
}

// Función principal del parser
ProgramNode *parse(const char *source_code)
{
    // Inicializar el lexer
    init_lexer(source_code);

    // Preparar los tokens iniciales
    current_token = get_next_token();
    peek_token = get_next_token();

    // Crear el nodo programa
    ProgramNode *program = new_program();

    // Parsear declaraciones hasta el final del archivo
    while (current_token.type != TOKEN_EOF)
    {
        // Ignorar comentarios
        if (current_token.type == TOKEN_COMMENT)
        {
            next_token();
            continue;
        }

        ASTNode *statement = parse_statement();
        if (statement != NULL)
        {
            add_statement(program, statement);
        }
        next_token();
    }

    return program;
}

// Liberar la memoria del AST
void free_ast(ASTNode *node)
{
    if (node == NULL)
        return;

    switch (node->type)
    {
    case NODE_PROGRAM:
    {
        ProgramNode *program = (ProgramNode *)node;
        for (int i = 0; i < program->count; i++)
        {
            free_ast(program->statements[i]);
        }
        free(program->statements);
        break;
    }
    case NODE_LET_STATEMENT:
    {
        LetStatementNode *let = (LetStatementNode *)node;
        free_ast((ASTNode *)let->identifier);
        free_ast(let->value);
        break;
    }
    case NODE_ECHO_STATEMENT:
    {
        EchoStatementNode *echo = (EchoStatementNode *)node;
        free_ast(echo->expression);
        break;
    }
    case NODE_BINARY_EXPR:
    {
        BinaryExprNode *expr = (BinaryExprNode *)node;
        free_ast(expr->left);
        free_ast(expr->right);
        break;
    }
    case NODE_IDENTIFIER:
    {
        IdentifierNode *id = (IdentifierNode *)node;
        free(id->name);
        break;
    }
    case NODE_NUMBER:
    case NODE_EXPRESSION:
        // No hay recursos adicionales que liberar
        break;
    case NODE_STRING:
    {
        StringNode *str = (StringNode *)node;
        free(str->value); // Liberar la cadena duplicada
        break;
    }
    }

    free(node);
}

// Imprimir el AST para depuración
void print_ast(ASTNode *node, int indent)
{
    if (node == NULL)
        return;

    char spaces[256] = {0};
    for (int i = 0; i < indent; i++)
    {
        spaces[i] = ' ';
    }

    switch (node->type)
    {
    case NODE_PROGRAM:
    {
        ProgramNode *program = (ProgramNode *)node;
        printf("%sPrograma:\n", spaces);
        for (int i = 0; i < program->count; i++)
        {
            print_ast(program->statements[i], indent + 2);
        }
        break;
    }
    case NODE_LET_STATEMENT:
    {
        LetStatementNode *let = (LetStatementNode *)node;
        printf("%sLet:\n", spaces);
        printf("%s  Identificador: %s\n", spaces, let->identifier->name);
        printf("%s  Valor:\n", spaces);
        print_ast(let->value, indent + 4);
        break;
    }
    case NODE_ECHO_STATEMENT:
    {
        EchoStatementNode *echo = (EchoStatementNode *)node;
        printf("%sEcho:\n", spaces);
        print_ast(echo->expression, indent + 2);
        break;
    }
    case NODE_BINARY_EXPR:
    {
        BinaryExprNode *expr = (BinaryExprNode *)node;
        printf("%sExpresión Binaria:\n", spaces);
        printf("%s  Operador: %d\n", spaces, expr->operator);
        printf("%s  Izquierda:\n", spaces);
        print_ast(expr->left, indent + 4);
        printf("%s  Derecha:\n", spaces);
        print_ast(expr->right, indent + 4);
        break;
    }
    case NODE_IDENTIFIER:
    {
        IdentifierNode *id = (IdentifierNode *)node;
        printf("%sIdentificador: %s\n", spaces, id->name);
        break;
    }
    case NODE_NUMBER:
    {
        NumberNode *num = (NumberNode *)node;
        printf("%sNúmero: %d\n", spaces, num->value);
        break;
    }
    case NODE_EXPRESSION:
        printf("%sExpresión\n", spaces);
        break;
    case NODE_STRING:
    {
        StringNode *str = (StringNode *)node;
        printf("%sCadena: \"%s\"\n", spaces, str->value);
        break;
    }
    }
}