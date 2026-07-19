#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "runtime/value.h"

typedef enum {
    SYMBOL_GLOBAL,
    SYMBOL_LOCAL
} SymbolType;

typedef struct Symbol {
    char *name;
    int slot;
    SymbolType type;
    struct Symbol *next;   
} Symbol;

typedef struct SymbolTable {
    int depth;
    struct SymbolTable *parent;
    Symbol *head;
    int count;
} SymbolTable;

SymbolTable* symbolTable_create(SymbolTable *parent, int depth);
Symbol* symbolTable_find(SymbolTable *table, const char *name);
int symbolTable_add(SymbolTable *table, const char *name, SymbolType type, int slot);
void symbolTable_free(SymbolTable* table);

#endif