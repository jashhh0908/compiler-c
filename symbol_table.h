#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

typedef struct SymbolTable SymbolTable;

SymbolTable* symbolTable_create(void);
void symbolTable_set(SymbolTable* table, const char *name, int val);
int symbolTable_get(SymbolTable* table, const char* name);
void symbolTable_free(SymbolTable* table);

#endif