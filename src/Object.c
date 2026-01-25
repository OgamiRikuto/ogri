#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "List.h"
#include "Object.h"

/**
 * Objectのメモリ確保を行います。
 */
static Object* new_object()
{
    Object* object = malloc(sizeof(Object));
    if(object == NULL) {
        fprintf(stderr, "Runtime Error: Failed to make Object.\n");
        exit(EXIT_FAILURE);
    }
    return object;
}

/**
 * 整数のオブジェクトを作成します。
 */
Object* new_int(long value)
{
    Object* obj = new_object();
    obj->type = INTEGER;
    obj->integer = value;
    return obj;
}

/**
 * 実数のオブジェクトを作成します。
 */
Object* new_float(double value)
{
    Object* obj = new_object();
    obj->type = FLOAT;
    obj->real = value;
    return obj;
}

/**
 * 文字列のオブジェクトを作成します。
 */
Object* new_string(char* string)
{
    Object* obj = new_object();
    obj->type = STRING;
    obj->string = strdup(string);
    return obj;
}

/**
 * 真偽値のオブジェクトを作成します。
 */
Object* new_bool(bool boolean)
{
    Object* obj = new_object();
    obj->type = BOOL;
    obj->boolean = boolean;
    return obj;
}

/**
 * 配列のオブジェクトを作成します。
 */
Object* new_array(List* list)
{
    Object* obj = new_object();
    obj->type = LIST;
    obj->list = list;
    return obj;
}

/**
 * 関数のオブジェクトを作成します。
 */
Object* new_func(Ast* params, Ast* block, Environment* env)
{
    Object* obj = new_object();
    obj->type = FUNCTION;
    obj->func = malloc(sizeof(Function));
    if(obj->func == NULL) {
        fprintf(stderr, "Runtime Error: Failed to make Object.\n");
        exit(EXIT_FAILURE);
    }
    obj->func->params = params;
    obj->func->block = block;
    obj->func->env = env;
    return obj;
}

/**
 * returnが返すオブジェクトのオブジェクトを作成します。
 */
Object* new_result(Object* result)
{
    Object* obj = new_object();
    obj->type = RETURN;
    obj->result = result;
    return obj;
}

/**
 * breakのオブジェクトを作成します。
 */
Object* new_break(void)
{
    Object* obj = new_object();
    obj->type = BREAK;
    return obj;
}

/**
 * continueのオブジェクトを作成します。
 */
Object* new_continue(void)
{
    Object* obj = new_object();
    obj->type = CONTINUE;
    return obj;
}

/**
 * オブジェクトのメモリ解放を行います。
 */
void obj_free(Object* self)
{
    switch(self->type) {
        case STRING:
            free(self->string);
            break;
        case LIST:
            dList(self->list);
            break;
        case FUNCTION:
            free(self->func);
            break;
        case RETURN:
            obj_free(self->result);
            break;
        default: break;
    }
    free(self);
}

/**
 * オブジェクトをディープコピーします。
 */
Object* obj_copy(Object* self)
{   
    if(self == NULL) return NULL;
    switch(self->type) {
        case INTEGER:   return new_int(self->integer);
        case FLOAT:     return new_float(self->real);
        case STRING:    return new_string(self->string);
        case BOOL:      return new_bool(self->boolean);
        case LIST:      return new_array(self->list);
        case FUNCTION:  return new_func(self->func->params, self->func->block, self->func->env);
        case RETURN:     return new_result(self->result);
        default: return NULL;
    }
}

/**
 * リストを文字列に変換します。
 */
static char* list_toString(Object* self)
{
    char buffer[1 << 17] = "[";
    int size = getSize(self->list);

    for (int i = 0; i < size; i++) {
        Object* item = NULL;
        LIST_ERROR getErr = getAt(self->list, i, Object*, &item);
        
        if (getErr == LIST_OK && item != NULL) {
            char* item_str = obj_toString(item);
            strncat(buffer, item_str, sizeof(buffer) - strlen(buffer) - 1);
            free(item_str);
        }
        if (i < size - 1) {
            strncat(buffer, ", ", sizeof(buffer) - strlen(buffer) - 1);
        }
    }

    strncat(buffer, "]", sizeof(buffer) - strlen(buffer) - 1);
    
    return strdup(buffer);
}

/**
 * オブジェクトを文字列に変換します。
 */
char* obj_toString(Object* self)
{
    char buffer[1024];
    if (self == NULL) return strdup("null");

    switch (self->type) {
        case INTEGER: sprintf(buffer, "%ld", self->integer); break;
        case FLOAT:   sprintf(buffer, "%g", self->real);     break;
        case STRING:  return strdup(self->string);
        case BOOL:    return strdup(self->boolean ? "true" : "false");
        case LIST:    return list_toString(self);
        default:      sprintf(buffer, "<obj:%p>", (void*)self); break;
    }
    return strdup(buffer);
}

/**
 * オブジェクトを出力します。
 */
void print_object(Object* obj)
{
    if (obj == NULL) {
        fprintf(stderr,"null");
        return;
    }

    switch (obj->type) {
        case INTEGER:
            fprintf(stderr, "%ld", obj->integer);
            break;
        case FLOAT:
            fprintf(stderr, "%g", obj->real);
            break;
        case STRING:
            fprintf(stderr, "%s", obj->string);
            break;
        case BOOL:
            fprintf(stderr, obj->boolean ? "true" : "false");
            break;
        case LIST: {
            fprintf(stderr, "[");
            for (int i = 0; i < getSize(obj->list); i++) {
                Object* item = NULL;
                getAt(obj->list, i, Object*, &item);
                print_object(item); 
                if (i < getSize(obj->list) - 1) fprintf(stderr, ", ");
            }
            fprintf(stderr, "]");
            break;
        }
        default:
            fprintf(stderr, "<object at %p>", (void*)obj);
            break;
    }
}


