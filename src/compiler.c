#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "compiler.h"

// Crear un nuevo chunk de bytecode
Chunk* init_chunk() {
    Chunk* chunk = malloc(sizeof(Chunk));
    chunk->capacity = 8;
    chunk->count = 0;
    chunk->code = malloc(sizeof(uint8_t) * chunk->capacity);
    chunk->constants_capacity = 8;
    chunk->constants_count = 0;
    chunk->constants = malloc(sizeof(Value) * chunk->constants_capacity);
    return chunk;
}

// Escribir un byte en el chunk
void chunk_write(Chunk* chunk, uint8_t byte) {
    if (chunk->capacity <= chunk->count) {
        int old_capacity = chunk->capacity;
        chunk->capacity = old_capacity * 2;
        chunk->code = realloc(chunk->code, sizeof(uint8_t) * chunk->capacity);
    }
    
    chunk->code[chunk->count] = byte;
    chunk->count++;
}

// Agregar una constante al chunk
int add_constant(Chunk* chunk, Value value) {
    if (chunk->constants_capacity <= chunk->constants_count) {
        int old_capacity = chunk->constants_capacity;
        chunk->constants_capacity = old_capacity * 2;
        chunk->constants = realloc(chunk->constants, sizeof(Value) * chunk->constants_capacity);
    }
    
    chunk->constants[chunk->constants_count] = value;
    return chunk->constants_count++;
}

// Liberar la memoria de un chunk
void free_chunk(Chunk* chunk) {
    for (int i = 0; i < chunk->constants_count; i++) {
        if (chunk->constants[i].type == VAL_STRING) {
            free(chunk->constants[i].as.string);
        }
    }
    
    free(chunk->code);
    free(chunk->constants);
    free(chunk);
}

// Compilar una expresión
void compile_expression(Chunk* chunk, ASTNode* node, VM* vm) {
    if (node == NULL) return;
    
    switch (node->type) {
        case NODE_NUMBER: {
            NumberNode* num = (NumberNode*)node;
            Value value;
            value.type = VAL_NUMBER;
            value.as.number = num->value;
            int constant = add_constant(chunk, value);
            chunk_write(chunk, OP_CONSTANT);
            chunk_write(chunk, constant);
            break;
        }
        case NODE_IDENTIFIER: {
            IdentifierNode* id = (IdentifierNode*)node;
            // Buscar la variable en las globales
            int index = -1;
            for (int i = 0; i < vm->globals_count; i++) {
                if (strcmp(vm->global_names[i], id->name) == 0) {
                    index = i;
                    break;
                }
            }
            
            if (index == -1) {
                printf("Error: variable '%s' no definida\n", id->name);
                exit(1);
            }
            
            chunk_write(chunk, OP_LOAD);
            chunk_write(chunk, index);
            break;
        }
        case NODE_BINARY_EXPR: {
            BinaryExprNode* expr = (BinaryExprNode*)node;
            
            // Compilar operandos
            compile_expression(chunk, expr->left, vm);
            compile_expression(chunk, expr->right, vm);
            
            // Compilar operador
            switch (expr->operator) {
                case TOKEN_PLUS:  chunk_write(chunk, OP_ADD); break;
                case TOKEN_MINUS: chunk_write(chunk, OP_SUBTRACT); break;
                case TOKEN_STAR:  chunk_write(chunk, OP_MULTIPLY); break;
                case TOKEN_SLASH: chunk_write(chunk, OP_DIVIDE); break;
                default:
                    printf("Error: operador desconocido\n");
                    exit(1);
            }
            break;
        }
        case NODE_STRING: {
            StringNode* str_node = (StringNode*)node;
            Value value;
            value.type = VAL_STRING;
            value.as.string = strdup(str_node->value);  // Copiar la cadena
            int constant = add_constant(chunk, value);
            chunk_write(chunk, OP_CONSTANT);
            chunk_write(chunk, constant);
            break;
        }        
        default:
            printf("Error: tipo de expresión desconocido: %d\n", node->type);
            exit(1);
    }
}

// Compilar una instrucción
void compile_statement(Chunk* chunk, ASTNode* node, VM* vm) {
    if (node == NULL) return;
    
    switch (node->type) {
        case NODE_LET_STATEMENT: {
            LetStatementNode* let = (LetStatementNode*)node;
            
            // Compilar el valor
            compile_expression(chunk, let->value, vm);
            
            // Buscar si la variable ya existe
            int index = -1;
            for (int i = 0; i < vm->globals_count; i++) {
                if (strcmp(vm->global_names[i], let->identifier->name) == 0) {
                    index = i;
                    break;
                }
            }
            
            // Si no existe, crearla
            if (index == -1) {
                if (vm->globals_count >= vm->globals_capacity) {
                    int old_capacity = vm->globals_capacity;
                    vm->globals_capacity = old_capacity * 2;
                    vm->globals = realloc(vm->globals, sizeof(Value) * vm->globals_capacity);
                    vm->global_names = realloc(vm->global_names, sizeof(char*) * vm->globals_capacity);
                }
                
                index = vm->globals_count;
                vm->global_names[index] = strdup(let->identifier->name);
                vm->globals_count++;
            }
            
            // Almacenar el valor en la variable
            chunk_write(chunk, OP_STORE);
            chunk_write(chunk, index);
            break;
        }
        case NODE_ECHO_STATEMENT: {
            EchoStatementNode* echo = (EchoStatementNode*)node;
            
            // Compilar la expresión
            compile_expression(chunk, echo->expression, vm);
            
            // Emitir instrucción de impresión
            chunk_write(chunk, OP_PRINT);
            break;
        }
        default:
            printf("Error: tipo de instrucción desconocido: %d\n", node->type);
            exit(1);
    }
}

// Compilar el programa
Chunk* compile(ProgramNode* program) {
    Chunk* chunk = init_chunk();
    
    // Inicializar la VM para el proceso de compilación
    VM* vm = malloc(sizeof(VM));
    vm->globals_capacity = 8;
    vm->globals_count = 0;
    vm->globals = malloc(sizeof(Value) * vm->globals_capacity);
    vm->global_names = malloc(sizeof(char*) * vm->globals_capacity);
    
    // Compilar cada instrucción
    for (int i = 0; i < program->count; i++) {
        compile_statement(chunk, program->statements[i], vm);
    }
    
    // Agregar instrucción de finalización
    chunk_write(chunk, OP_HALT);
    
    // Liberar la VM temporal
    for (int i = 0; i < vm->globals_count; i++) {
        free(vm->global_names[i]);
    }
    free(vm->globals);
    free(vm->global_names);
    free(vm);
    
    return chunk;
}

// Inicializar la máquina virtual
VM* init_vm(Chunk* chunk) {
    VM* vm = malloc(sizeof(VM));
    vm->chunk = chunk;
    vm->ip = chunk->code;
    vm->stack_capacity = 256;
    vm->stack = malloc(sizeof(Value) * vm->stack_capacity);
    vm->stack_top = 0;
    vm->globals_capacity = 256;
    vm->globals = malloc(sizeof(Value) * vm->globals_capacity);
    vm->global_names = malloc(sizeof(char*) * vm->globals_capacity);
    vm->globals_count = 0;
    return vm;
}

// Liberar la memoria de la máquina virtual
void free_vm(VM* vm) {
    for (int i = 0; i < vm->globals_count; i++) {
        free(vm->global_names[i]);
    }
    free(vm->stack);
    free(vm->globals);
    free(vm->global_names);
    free(vm);
}

// Imprimir un valor
void print_value(Value value) {
    switch (value.type) {
        case VAL_NUMBER:
            printf("%d", value.as.number);
            break;
        case VAL_STRING:
            printf("%s", value.as.string);
            break;
    }
}

// Poner un valor en la pila
void push(VM* vm, Value value) {
    if (vm->stack_top >= vm->stack_capacity) {
        int old_capacity = vm->stack_capacity;
        vm->stack_capacity = old_capacity * 2;
        vm->stack = realloc(vm->stack, sizeof(Value) * vm->stack_capacity);
    }
    
    vm->stack[vm->stack_top] = value;
    vm->stack_top++;
}

// Sacar un valor de la pila
Value pop(VM* vm) {
    if (vm->stack_top <= 0) {
        printf("Error: pila vacía\n");
        exit(1);
    }
    
    vm->stack_top--;
    return vm->stack[vm->stack_top];
}

// Ejecutar la máquina virtual
void run_vm(VM* vm) {
    for (;;) {
        uint8_t instruction = *vm->ip++;
        
        switch (instruction) {
            case OP_CONSTANT: {
                uint8_t constant = *vm->ip++;
                push(vm, vm->chunk->constants[constant]);
                break;
            }
            case OP_ADD: {
                Value b = pop(vm);
                Value a = pop(vm);
                
                if (a.type == VAL_NUMBER && b.type == VAL_NUMBER) {
                    Value result;
                    result.type = VAL_NUMBER;
                    result.as.number = a.as.number + b.as.number;
                    push(vm, result);
                } else {
                    printf("Error: solo se pueden sumar números\n");
                    exit(1);
                }
                break;
            }
            case OP_SUBTRACT: {
                Value b = pop(vm);
                Value a = pop(vm);
                
                if (a.type == VAL_NUMBER && b.type == VAL_NUMBER) {
                    Value result;
                    result.type = VAL_NUMBER;
                    result.as.number = a.as.number - b.as.number;
                    push(vm, result);
                } else {
                    printf("Error: solo se pueden restar números\n");
                    exit(1);
                }
                break;
            }
            case OP_MULTIPLY: {
                Value b = pop(vm);
                Value a = pop(vm);
                
                if (a.type == VAL_NUMBER && b.type == VAL_NUMBER) {
                    Value result;
                    result.type = VAL_NUMBER;
                    result.as.number = a.as.number * b.as.number;
                    push(vm, result);
                } else {
                    printf("Error: solo se pueden multiplicar números\n");
                    exit(1);
                }
                break;
            }
            case OP_DIVIDE: {
                Value b = pop(vm);
                Value a = pop(vm);
                
                if (a.type == VAL_NUMBER && b.type == VAL_NUMBER) {
                    if (b.as.number == 0) {
                        printf("Error: división por cero\n");
                        exit(1);
                    }
                    
                    Value result;
                    result.type = VAL_NUMBER;
                    result.as.number = a.as.number / b.as.number;
                    push(vm, result);
                } else {
                    printf("Error: solo se pueden dividir números\n");
                    exit(1);
                }
                break;
            }
            case OP_PRINT: {
                Value value = pop(vm);
                print_value(value);
                printf("\n");
                break;
            }
            case OP_STORE: {
                uint8_t index = *vm->ip++;
                Value value = pop(vm);
                vm->globals[index] = value;
                break;
            }
            case OP_LOAD: {
                uint8_t index = *vm->ip++;
                push(vm, vm->globals[index]);
                break;
            }
            case OP_HALT:
                return;
            default:
                printf("Error: opcode desconocido: %d\n", instruction);
                exit(1);
        }
    }
}

// Imprimir chunk para depuración
void print_chunk(Chunk* chunk, const char* name) {
    printf("== %s ==\n", name);
    
    for (int i = 0; i < chunk->count;) {
        uint8_t instruction = chunk->code[i];
        printf("%04d ", i);
        
        switch (instruction) {
            case OP_CONSTANT:
                printf("OP_CONSTANT %d '", chunk->code[i + 1]);
                print_value(chunk->constants[chunk->code[i + 1]]);
                printf("'\n");
                i += 2;
                break;
            case OP_ADD:
                printf("OP_ADD\n");
                i += 1;
                break;
            case OP_SUBTRACT:
                printf("OP_SUBTRACT\n");
                i += 1;
                break;
            case OP_MULTIPLY:
                printf("OP_MULTIPLY\n");
                i += 1;
                break;
            case OP_DIVIDE:
                printf("OP_DIVIDE\n");
                i += 1;
                break;
            case OP_PRINT:
                printf("OP_PRINT\n");
                i += 1;
                break;
            case OP_STORE:
                printf("OP_STORE %d\n", chunk->code[i + 1]);
                i += 2;
                break;
            case OP_LOAD:
                printf("OP_LOAD %d\n", chunk->code[i + 1]);
                i += 2;
                break;
            case OP_HALT:
                printf("OP_HALT\n");
                i += 1;
                break;
            default:
                printf("Unknown opcode %d\n", instruction);
                i += 1;
                break;
        }
    }
}