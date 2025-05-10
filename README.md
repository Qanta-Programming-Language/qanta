# Qanta Programming Language

Qanta es un lenguaje de programación simple e interpretado, diseñado con fines educativos.

## Características

- Variables con asignación dinámica
- Operaciones aritméticas básicas (+, -, *, /)
- Soporte para strings y números enteros
- Comentarios de una línea
- Instrucción de salida (echo)

## Instalación

```bash
git clone https://github.com/Qanta-Programming-Language/qanta.git
cd qanta-programming-language
make
```

## Uso

```bash
make clean && make
```

```bash
./qanta examples/test.qt
./qanta examples/test.qt --debug  # Para modo debug
```

## Sintaxis

### Variables
```qt
let x = 42;
let mensaje = "Hola mundo";
```

### Operaciones
```qt
let a = 5 + 3 * 2;
let b = 10 - 5;
```

### Salida
```qt
echo "Hola mundo";
echo 42;
echo variable;
```

### Comentarios
```qt
# Esto es un comentario
```

## Ejemplos

Ver la carpeta `/examples` para más ejemplos de código.

## Licencia

MIT License - Ver archivo LICENSE para más detalles.
