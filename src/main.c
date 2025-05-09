#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "compiler.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("Uso: %s archivo.qt\n", argv[0]);
        return 1;
    }

    FILE* file = fopen(argv[1], "r");
    if (!file) {
        perror("No se pudo abrir el archivo");
        return 1;
    }

    // Leer el archivo de código fuente
    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    rewind(file);

    char* source = malloc(length + 1);
    fread(source, 1, length, file);
    source[length] = '\0';
    fclose(file);

    // Habilitar o deshabilitar la depuración
    int debug_mode = 0;
    if (argc > 2 && strcmp(argv[2], "--debug") == 0) {
        debug_mode = 1;
    }

    if (debug_mode) {
        printf("======= ANÁLISIS LÉXICO =======\n");
        // Inicializar el analizador léxico y mostrar los tokens
        init_lexer(source);
        Token token;
        do {
            token = get_next_token();
            printf("Token: %-15s [%s]\n", 
                (const char*[]) {
                    "LET", "ECHO", "IDENTIFIER", "NUMBER", "ASSIGN", "PLUS", 
                    "MINUS", "STAR", "SLASH", "SEMICOLON", "COMMENT", "EOF", "UNKNOWN"
                }[token.type], token.text);
            free_token(token);
        } while (token.type != TOKEN_EOF);
        
        printf("\n======= ANÁLISIS SINTÁCTICO =======\n");
    }
    
    // Parsear el código fuente
    ProgramNode* program = parse(source);
    
    if (debug_mode) {
        // Mostrar el AST
        print_ast((ASTNode*)program, 0);
        
        printf("\n======= COMPILACIÓN =======\n");
    }
    
    // Compilar el programa
    Chunk* chunk = compile(program);
    
    if (debug_mode) {
        // Mostrar el bytecode generado
        print_chunk(chunk, "Programa Principal");
        
        printf("\n======= EJECUCIÓN =======\n");
    }
    
    // Ejecutar el programa
    VM* vm = init_vm(chunk);
    run_vm(vm);
    
    // Liberar memoria
    free_vm(vm);
    free_chunk(chunk);
    free_ast((ASTNode*)program);
    free(source);
    
    return 0;
}