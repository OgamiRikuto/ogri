#include <stdio.h>
#include <string.h>
#include "built_in_functions.h"
#include "Environment.h"
#include "List.h"
#include "Object.h"

static BuiltinDef builtins[] = {
    {"say", builtin_say},
    {"says", builtin_says},
    {"to_int", builtin_to_int},
    {"listen", builtin_listen},
    {"range", builtin_range},
    {"len", builtin_len},
    {"push", builtin_push},
    {"pop", builtin_pop},
    {NULL, NULL}
};

/**
 * グローバル環境にビルトイン関数を定義します。
 */
void set_builtins(Environment* env)
{
    for(int index = 0; builtins[index].name != NULL; index++) {
        Object* func = malloc(sizeof(Object));
        func->type = BUILT_IN_FUNCTION;
        func->b_func = builtins[index].func;
        env_set(env, builtins[index].name, func);
    }
}

/**
 * sayの本体です。
 * 第2引数によって最後に改行するか否かを決めます。
 */
static Object* internal_print(List* args, bool newline)
{
    if(args == NULL) {
        if(newline) printf("\n");
        return new_int(0);
    }
    int len = 0;
    int size = getSize(args);
    for(int index = 0; index < size; index++) {
        Object* arg;
        getAt(args, index, Object*, &arg);

        char* string = obj_toString(arg);
        for(int index = 0; string[index] != '\0'; index++) {
            if(string[index] == '\\' && string[index+1] == 'n') {
                putchar('\n');
                index++;
            } else putchar(string[index]);
        }
        len += strlen(string);
        free(string);

        if(index < size - 1) printf(" ");
    }
    if(newline) printf("\n");

    fflush(stdout);
    return new_int((long)len);
}

/**
 * 出力です。最後に改行されます。
 */
Object* builtin_say(List* args)
{
    return internal_print(args, true);
}

/**
 * 出力です。最後に改行されません。
 */
Object* builtin_says(List* args)
{
    return internal_print(args, false);
}

/**
 * 引数を整数型に変換します。
 * 変換できないものが来た場合は0という整数を返します。
 */
Object* builtin_to_int(List* args)
{
    if(args == NULL || getSize(args) < 1) return new_int(0);
    
    Object* arg;
    getAt(args, 0, Object*, &arg);

    if(arg->type == STRING)
        return new_int(atol(arg->string));
    else if(arg->type == FLOAT)
        return new_int((long)arg->real);

    return new_int(0);
}

/**
 * 入力を受け付けます。
 * 空白で複数の入力を受け付けます。
 * 複数の入力を受け取った場合はその入力をのリストを返します。
 */
Object* builtin_listen(List* args)
{
    char buffer[1<<16];
    if(fgets(buffer, sizeof(buffer), stdin) == NULL) return new_string("");

    buffer[strcspn(buffer, "\n")] = '\0';

    Object* converter = NULL;
    if (args != NULL && getSize(args) > 0) {
        getAt(args, 0, Object*, &converter);
        if(converter->type != BUILT_IN_FUNCTION) converter = NULL;
    }

    if (converter != NULL && strchr(buffer, ' ') != NULL) {
        List* result_list = newList(Object*);
        
        char* token = strtok(buffer, " ");
        while (token != NULL) {
            Object* item = new_string(token);

            if (converter != NULL) {
                List* tmp_args = newList(Object*);
                add(tmp_args, &item);
                item = converter->b_func(tmp_args);
                dList(tmp_args);
            }

            add(result_list, &item);
            token = strtok(NULL, " ");
        }
        return new_array(result_list);
    } else if(converter != NULL) {
        Object* result = new_string(buffer);
        List* tmp_args = newList(Object*);
        add(tmp_args, &result);
        result = converter->b_func(tmp_args);
        dList(tmp_args);
        return result;
    } else {
        return new_string(buffer);
    }
}

/**
 * 整数のリストを返す関数です。
 * 引数が1つの場合は0から引数の数まで1ずつ数えた数値のリストを返します。
 * 引数が2つの場合は第1引数から第2引数まで1つずつ数え上げた数値のリストを返します。
 * 引数が3つの場合は第1引数から第2引数まで第3引数の間隔で数え上げた数値のリストを変えします。
 * 第3引数が0だった場合は強制的に1として実行されます。
 */
Object* builtin_range(List* args)
{
    if(args == NULL || getSize(args) < 1) {
        return new_array(newList(Object*));
    }

    long start = 0;
    long end = 0;
    long step = 1;

    if(getSize(args) == 1) {
        Object* arg;
        getAt(args, 0, Object*, &arg);
        if (arg->type != INTEGER) {
            fprintf(stderr, "Runtime Error: range requires integer arguments.\n");
            return new_array(newList(Object*));
        }
        end = arg->integer;
    } else if(getSize(args) == 2) {
        Object *start_obj, *end_obj;
        getAt(args, 0, Object*, &start_obj);
        getAt(args, 1, Object*, &end_obj);
    
        if (start_obj->type != INTEGER || end_obj->type != INTEGER) {
            fprintf(stderr, "Runtime Error: range requires integer arguments.\n");
            return new_array(newList(Object*));
        }
    
        start = start_obj->integer;
        end = end_obj->integer;
    } else if(getSize(args) == 3) {
        Object *start_obj, *end_obj, *step_obj;
        getAt(args, 0, Object*, &start_obj);
        getAt(args, 1, Object*, &end_obj);
        getAt(args, 2, Object*, &step_obj);
    
        if (start_obj->type != INTEGER || end_obj->type != INTEGER || step_obj->type != INTEGER) {
            fprintf(stderr, "Runtime Error: range requires integer arguments.\n");
            return new_array(newList(Object*));
        }
    
        start = start_obj->integer;
        end = end_obj->integer;
        step = step_obj->integer;
    }

    if(step == 0) step = 1;

    List* result = newList(Object*);

    if(step > 0) {
        for(long index = start; index < end; index += step) {
            Object* value = new_int(index);
            add(result, &value);
        }
    } else {
        for(long index = start; index > end; index += step) {
            Object* value = new_int(index);
            add(result, &value);
        }
    }

    return new_array(result);
}

/**
 * 受け取ったリストの長さを返します。
 */
Object* builtin_len(List* args)
{
    if(args == NULL || getSize(args) < 1) 
        return new_int(0);

    int size = getSize(args);
    if(size == 1) {
        Object* arg;
        getAt(args, 0, Object*, &arg);
        if(arg->type == LIST)
            return new_int((long) getSize(arg->list));
        if(arg->type == STRING)
            return new_int((long) strlen(arg->string));

    }
    return new_int((long)size);
}

/**
 * 第1引数に受け取った配列の末尾に第2引数の要素を追加します。
 */
Object* builtin_push(List* args)
{
    if(args == NULL || getSize(args) < 2) {
        fprintf(stderr, "Runtime Error: append requires at least 2 arguments.\n");
        exit(EXIT_FAILURE);
    }

    Object *list, *value;
    getAt(args, 0, Object*, &list);
    getAt(args, 1, Object*, &value);

    if(list->type != LIST) {
        fprintf(stderr, "Runtime Error: first argument requires list.\n");
        exit(EXIT_FAILURE);
    }

    add(list->list, &value);

    return new_array(list->list);
}

/**
 * 第1引数の配列の最後の要素を削除し、返します。
 */
Object* builtin_pop(List* args)
{
    if(args == NULL || getSize(args) < 1) {
        fprintf(stderr, "Runtime Error: push requires at least 1 arguments.\n");
        exit(EXIT_FAILURE);
    }

    Object* list;
    getAt(args, 0, Object*, &list);

    if(list->type != LIST) {
        fprintf(stderr, "Runtime Error: pop requires list.\n");
        exit(EXIT_FAILURE);
    }
    int size = getSize(list->list);
    if(size == 0) return new_int(0);

    Object* last;
    getAt(list->list, size-1, Object*, &last);
    removeAt(list->list, size-1);
    return last;
}
