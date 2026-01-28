/**
 * オブジェクトです。
 * 整数、実数、文字列、真偽値、関数、リストは全てオブジェクトとして扱われます。
 */
#ifndef __OBJECT_H__
#define __OBJECT_H__

#include <stdbool.h>
typedef struct Ast Ast;
typedef struct environment Environment;
typedef struct _list List;

typedef enum {
    INTEGER,
    FLOAT,
    STRING,
    BOOL,
    LIST,
    FUNCTION,
    BUILT_IN_FUNCTION,
    RETURN,
    BREAK,
    CONTINUE,
    NONE
} ObjectType;

typedef struct func Function;
typedef struct object Object;

typedef Object* (*built_in_function)(List* args);

struct func {
    Ast* params;
    Ast* block;
    Environment* env;
};
struct object {
    ObjectType type;
    union {
        long integer;
        double real;
        char* string;
        bool boolean;
        List* list;
        Function* func;
        built_in_function b_func;
        Object* result;
    };
};

Object* new_int(long);
Object* new_float(double);
Object* new_string(char*);
Object* new_bool(bool);
Object* new_array(List*);
Object* new_func(Ast*, Ast*, Environment*);
Object* new_result(Object*);
Object* new_break(void);
Object* new_continue(void);

void obj_free(Object*);
Object* obj_copy(Object*);
char* obj_toString(Object*);
void print_object(Object*);

#endif  /* __OBJECT_H__ */
