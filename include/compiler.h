#ifndef COMPILER_H
#define COMPILER_H

#include <stdint.h>
#include "parser.h"

// Opcodes (códigos de operación) para el bytecode
typedef enum {
    OP_CONSTANT,  // Cargar una constante
    OP_ADD,       // Suma
    OP_SUBTRACT,  // Resta
    OP_MULTIPLY,  // Multiplicación
    OP_DIVIDE,    // División
    OP_PRINT,     // Mostrar valor
    OP_STORE,     // Guardar en variable
    OP_LOAD,      // Cargar variable
    OP_HALT       // Finalizar ejecución
} OpCode;

// Estructura para almacenar una constante
typedef struct {
    enum {
        VAL_NUMBER,
        VAL_STRING
    } type;
    union {
        int number;
        char* string;
    } as;
} Value;

// Estructura para un chunk de bytecode
typedef struct {
    uint8_t* code;     // Array de bytecode
    int capacity;      // Capacidad del array
    int count;         // Cantidad de bytes usados
    Value* constants;  // Constantes usadas en el código
    int constants_capacity;
    int constants_count;
} Chunk;

// Estructura para una máquina virtual
typedef struct {
    Chunk* chunk;          // Chunk actual
    uint8_t* ip;           // Instruction pointer (puntero de instrucción)
    Value* stack;          // Pila de valores
    int stack_capacity;
    int stack_top;         // Tope de la pila
    Value* globals;        // Variables globales
    int globals_capacity;
    char** global_names;   // Nombres de variables globales
    int globals_count;
} VM;

// Funciones del compilador
Chunk* compile(ProgramNode* program);
void free_chunk(Chunk* chunk);

// Funciones de la máquina virtual
VM* init_vm(Chunk* chunk);
void free_vm(VM* vm);
void run_vm(VM* vm);

// Funciones de utilidad
void chunk_write(Chunk* chunk, uint8_t byte);
int add_constant(Chunk* chunk, Value value);
void print_chunk(Chunk* chunk, const char* name);
void print_value(Value value);

#endif