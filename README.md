# compiler-c

compiler-c is a compiler-style programming language implementation written in C.

The project has evolved from a tree-walking interpreter into a **hybrid system with a bytecode compiler and stack-based virtual machine (VM)**, closely resembling real-world language runtimes like Python and Java.
## Features

### 1. Frontend (Compiler)

- Lexer (tokenizes source code)
- Recursive-descent parser with proper operator precedence
- Abstract Syntax Tree (AST) construction

### 2. Backend

#### a) Tree-Walking Interpreter
- Direct AST execution
- Useful for debugging and validation

#### b) Bytecode Compiler + Virtual Machine (new)
- AST → Bytecode compilation
- Stack-based Virtual Machine execution
- Instruction-based execution model
- Constant pool for efficient value storage
## Architecture
The system follows a compiler-style pipeline with two execution strategies:

```
Source Code
   ↓
Lexer (characters → tokens)
   ↓
Parser (tokens → AST)
   ↓
───────────────┬────────────────
               ↓
     Tree-Walking Interpreter
               ↓
          Runtime Values

OR

               ↓
        Bytecode Compiler
     (AST → Instructions)
               ↓
     Virtual Machine (VM)
   (stack-based execution)
               ↓
          Runtime Values
```
### Components Breakdown

- **Lexer** → Converts raw source code into tokens  
- **Parser** → Builds an Abstract Syntax Tree (AST) using recursive descent  
- **AST** → Structured representation of the program  
- **Interpreter** → Executes the AST directly  
- **Code Generator** → Compiles AST into bytecode instructions  
- **Chunk / Instruction Set** → Stores bytecode and constants  
- **Virtual Machine (VM)** → Executes bytecode using a stack-based model  
- **Symbol Table** → Tracks variables and their values  
- **Value System** → Handles runtime data types  

This architecture mirrors real-world compilers and allows both direct interpretation and compiled execution via a virtual machine.

## Example

Input:
```text
print(10 > 5);
print(10 < 5);
print(10 >= 5);
print(10 <= 5);
print(10 == 10);
print(10 != 10);
``` 
Output:
```text
true
false
true
false
true
false
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
  gcc ast.c chunk.c codegen.c lexer.c main.c parser.c symbol_table.c value.c vm.c -o main
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