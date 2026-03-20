#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H
#include "value.h"
typedef struct SymbolTable SymbolTable;

SymbolTable* symbolTable_create(void);
int symbolTable_find(SymbolTable *table, const char *name);
int symbolTable_add(SymbolTable *table, const char *name);
int symbolTable_getIndex(SymbolTable *table, const char *name);
void symbolTable_free(SymbolTable* table);

#endif