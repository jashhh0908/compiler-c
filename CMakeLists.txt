cmake_minimum_required(VERSION 3.10)
project(CompilerC C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compile options for warnings and debugging
if(MSVC)
    add_compile_options(/W4 /WX-)
else()
    add_compile_options(-Wall -Wextra -Wpedantic -g)
endif()

# Source files (current flat structure)
set(SOURCES
    main.c
    lexer.c
    parser.c
    ast.c
    symbol_table.c
    codegen.c
    chunk.c
    value.c
    vm.c
    evaluator.c
)

# Add the executable target
add_executable(compiler-c ${SOURCES})

# Include headers from the current directory
target_include_directories(compiler-c PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
