# compiler-c

compiler-c is a compiler-style language implementation written in C.

The project currently includes a full compiler frontend (lexer, recursive-descent
parser, and AST construction) and a tree-walking interpreter backend for execution. The architecture is intentionally designed to mirror real compiler structure and to enable future backends such as bytecode generation.

## Features

- Lexer that tokenizes source code into meaningful tokens
- Recursive-descent parser with correct operator precedence
- Abstract Syntax Tree (AST) construction
- Tree-walking interpreter backend
- Symbol table for variable storage
- Integer arithmetic (`+ - * /`)
- String literals and string concatenation
- Boolean values (`true`, `false`)
- Equality operators (`==`, `!=`)
- Relational operators (`<`, `<=`, `>`, `>=`)
- Logical operators (`&&`, `||`) with short-circuit evaluation
- Variable assignment
- `if / else` statements (nested supported)
- `while` loops
- `break` with proper propagation semantics
- Runtime type checking with clear error messages

## Architecture
The project is structured like a real compiler-style system:

Source Code  
→ Lexer (characters → tokens)  
→ Parser (tokens → AST)  
→ Interpreter Backend (AST → runtime values)  
→ Symbol Table (variable storage)

This separation ensures clear responsibility between lexical analysis,
syntax parsing, and semantic evaluation.

## Example

Input:
```text
i = 0;
while (i < 3) {
    print i;
    i = i + 1;
}
``` 
Output:
```text
0
1
2
```

## Run Locally

Clone the project

```bash
  git clone https://github.com/jashhh0908/compiler-c.git
```

Go to the project directory

```bash
  cd compiler-c
```

Compile the project

```bash
  gcc main.c parser.c lexer.c ast.c evaluator.c symbol_table.c value.c -o main
```

Run the executable

On Windows (Command Prompt):
```bash
  main.exe
```
On VSCode Terminal:
```bash
  ./main
```
