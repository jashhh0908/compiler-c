#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "symbol_table.h"

SymbolTable* symbolTable_create(SymbolTable *parent, int depth) {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    if(!table) {
        printf("error allocating memory to table\n");
        return NULL;
    } 
    table->parent = parent;
    table->depth = depth;
    table->head = NULL;
    table->count = 0;
    return table;
}

Symbol* symbolTable_find(SymbolTable *table, const char *name) {
    SymbolTable *current_scope = table;
    
    while(current_scope != NULL) {
        Symbol *symbol = current_scope->head;
        while(symbol != NULL) {
            if(strcmp(symbol->name, name) == 0) {
               return symbol;
            }
            symbol = symbol->next;
        }
        current_scope = current_scope->parent;
    }
    return NULL;
}

int symbolTable_add(SymbolTable *table, const char *name, SymbolType type, int slot){
    Symbol *newSymbol = malloc(sizeof(Symbol));
    if(!newSymbol) {
        printf("error allocating memory to symbol during add()\n");
        return -1;
    }
    newSymbol->name = malloc(strlen(name) + 1);
    strcpy(newSymbol->name, name);

    newSymbol->slot = slot;
    newSymbol->type = type;
    
    newSymbol->next = table->head;
    table->head = newSymbol;
    table->count++;
    return slot;
}

// int symbolTable_getIndex(SymbolTable *table, const char *name) {
//     int idx;
//     idx = symbolTable_find(table, name);
//     if(idx != -1) {
//         return idx;
//     } else {
//         idx = symbolTable_add(table, name);
//         return idx;
//     }
// }

void symbolTable_free(SymbolTable* table) {
    if(!table) return;
    Symbol *current = table->head;
    while(current != NULL) {
        Symbol *next = current->next;
        free(current->name); //name is allocated space separately hence it is freed separately 
        free(current);
        current = next;
    }
    free(table);
}