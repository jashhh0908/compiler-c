#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "symbol_table.h"
typedef struct Symbol {
    char *name;
    Value value;
    struct Symbol *next;   
} Symbol;

typedef struct SymbolTable {
    Symbol *head;
} SymbolTable;

SymbolTable *symbolTable_create(void) {
    SymbolTable *table = malloc(sizeof(SymbolTable));
    if(!table) {
        printf("error allocating memory to table\n");
        return NULL;
    } 
    table->head = NULL;
    return table;
}

void symbolTable_set(SymbolTable* table, const char *name, Value val) {
    Symbol *current = table->head;
    while(current != NULL) {
        if(strcmp(current->name, name) == 0) {
            free_value(&current->value);
            current->value = value_copy(&val);
            return;
        }
        current = current->next;
    }
    
    Symbol *newSymbol = malloc(sizeof(Symbol));

    newSymbol->name = malloc(strlen(name) + 1);
    strcpy(newSymbol->name, name);
    newSymbol->value = value_copy(&val);
    newSymbol->next = table->head;

    table->head = newSymbol;
}

Value symbolTable_get(SymbolTable* table, const char *name) {
    Symbol *current = table->head;
    while(current != NULL) {
        if(strcmp(current->name, name) == 0) {
            return value_copy(&current->value);
        }
        current = current->next;
    }
    
    printf("Error: Undefined variable %s\n", name);
    exit(1);
}

void symbolTable_free(SymbolTable* table) {
    Symbol *current = table->head;
    while(current != NULL) {
        Symbol *next = current->next;
        free(current->name); //name is allocated space separately hence it is freed separately 
        free_value(&current->value);
        free(current);
        current = next;
    }
    free(table);
}