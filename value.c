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

Value value_copy(const Value *v) {
    if(v->type == VALUE_INT) {
        return value_int(v->val);
    } else if(v->type == VALUE_STRING) {
        return value_string(v->str);
    }
}

void free_value(Value *v) {
    if(v->type == VALUE_STRING) {
        free(v->str);
    }   
}
