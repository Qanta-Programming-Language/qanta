# Qanta Programming Language

Qanta is a programming language designed for backend use.

## Features

- Dynamically allocated variables
- Basic arithmetic operations (+, -, *, /)
- Support for strings and integers
- Single-line comments
- Output statement (echo)

## Installation

```bash
git clone https://github.com/Qanta-Programming-Language/qanta.git
cd qanta-programming-language
make
```

## Usage

```bash
./qanta examples/test.qt
./qanta examples/test.qt --debug # For debug mode
```

## Syntax

### Variables
```qt
let x = 42;
let message = "Hello world";
```

### Operations
```qt
let a = 5 + 3 * 2;
let b = 10 - 5;
```

### Output
```qt
echo "Hello world";
echo 42;
echo variable;
```

### Comments
```qt
# This is a comment
```

## Examples

See the `/examples` folder for more code examples.

## License

MIT License - See the LICENSE file for details.