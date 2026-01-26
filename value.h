#ifndef VALUE_H
#define VALUE_H

typedef enum {
    VALUE_INT,
    VALUE_STRING
} ValueType;

typedef struct {
    ValueType type;
    union {
        int val;
        char *str;
    };
} Value;

Value value_int(int x);
Value value_string(const char *str);
Value value_copy(const Value *v);
void free_value(Value *v);

#endif