/**
 * ビルトイン関数を定義します。
 */
#ifndef __BUILT_IN_FUNCTIONS_H__
#define __BUILT_IN_FUNCTIONS_H__

typedef struct environment Environment;
typedef struct _list List;
typedef struct object Object;

typedef Object* (*built_in_function)(List* args);
typedef struct builtins BuiltinDef;

struct builtins {
    const char* name;
    built_in_function func;
};

void set_builtins(Environment*);
Object* builtin_say(List*);
Object* builtin_says(List*);
Object* builtin_to_int(List*);
Object* builtin_listen(List*);
Object* builtin_range(List*);
Object* builtin_len(List*);
Object* builtin_push(List*);
Object* builtin_pop(List*);

#endif /* BUILT_IN_FUNCTIONS_H__ */
