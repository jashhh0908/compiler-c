#include "value.h"
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

Value value_int(int x) {
    Value v;
    v.type = VALUE_INT;
    v.val = x;
    return v;
}

Value value_string(const char *str) {
    Value v;
    v.type = VALUE_STRING;
    v.str = malloc(strlen(str) + 1);
    strcpy(v.str, str);
    return v;   
}

Value value_bool(int b) {
    Value v;
    v.type = VALUE_BOOL;
    v.bool_val = b ? 1 : 0;
    return v;
}

Value value_copy(const Value *v) {
    switch(v->type) {
        case VALUE_INT: return value_int(v->val);
        case VALUE_STRING: return value_string(v->str); 
        case VALUE_BOOL: return value_bool(v->bool_val); 
        default: printf("Unknown value type\n"); exit(1);
    }
}

void free_value(Value *v) {
    if(v->type == VALUE_STRING) {
        free(v->str);
        v->str = NULL;  
    }   
}
