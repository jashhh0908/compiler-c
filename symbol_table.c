#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "symbol_table.h"
typedef struct Symbol {
    char *name;
    int index;
    struct Symbol *next;   
} Symbol;

typedef struct SymbolTable {
    Symbol *head;
    int count;
} SymbolTable;

SymbolTable *symbolTable_create(void) {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    if(!table) {
        printf("error allocating memory to table\n");
        return NULL;
    } 
    table->head = NULL;
    table->count = 0;
    return table;
}

int symbolTable_find(SymbolTable *table, const char *name) {
    Symbol *current = table->head;
    
    while(current != NULL) {
        if(strcmp(current->name, name) == 0) {
            return current->index;
        }
        current = current->next;
    }
    return -1;
}

int symbolTable_add(SymbolTable *table, const char *name) {
    Symbol *newSymbol = malloc(sizeof(Symbol));
    
    newSymbol->name = malloc(strlen(name) + 1);
    strcpy(newSymbol->name, name);

    newSymbol->index = table->count;
    table->count++;
    
    newSymbol->next = table->head;
    table->head = newSymbol;
    return newSymbol->index;
}

int symbolTable_getIndex(SymbolTable *table, const char *name) {
    int idx;
    idx = symbolTable_find(table, name);
    if(idx != -1) {
        return idx;
    } else {
        idx = symbolTable_add(table, name);
        return idx;
    }
}
void symbolTable_free(SymbolTable* table) {
    Symbol *current = table->head;
    while(current != NULL) {
        Symbol *next = current->next;
        free(current->name); //name is allocated space separately hence it is freed separately 
        free(current);
        current = next;
    }
    free(table);
}