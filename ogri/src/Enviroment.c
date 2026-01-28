#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "Dictionary.h"
#include "Environment.h"

#define DEFAULT_DICT_CAPACITY 100

/**
 * コンストラクタです。
 */
Environment* newEnv(Environment* outer)
{
    Environment* env = malloc(sizeof(Environment));
    if(env == NULL) {
        fprintf(stderr, "\nRuntime Error: cannot make environment...\n");
        exit(EXIT_FAILURE);
    }
    env->table = newDict(DEFAULT_DICT_CAPACITY);
    env->outer = outer;
    return env;

}

/**
 * スコープに識別子を定義します。
 * すでにある識別子だった場合は代入します。
 */
void env_set(Environment* self, const char* key, Object* value)
{
    Environment* current = self;

    while(current != NULL) {
        HashEntry result = dict_get(current->table, key);
        if(result.status == OCCUPIED) {
            dict_set(current->table, key, value);
            return;
        }
        current = current->outer;
    }
    dict_set(self->table, key, value);
}

/**
 * 識別子を定義します。(辞書に登録します。)
 */
void env_define(Environment* self, const char* key, Object* value)
{
    dict_set(self->table, key, value);
}

/**
 * 再代入します。
 */
void env_assign(Environment* self, const char* key, Object* value, int line)
{
    if(self == NULL) {
        fprintf(stderr, "Runtime Error at %d: %s is not defined...\n", line, key);
        exit(EXIT_FAILURE);
    }
    HashEntry result = dict_get(self->table, key);
    if(result.status == OCCUPIED) dict_set(self->table, key, value);
    else env_assign(self->outer, key, value, line);
}

/**
 * 与えられた識別子からその値を返します。
 */
Object* env_get(Environment* self, const char* key, int line)
{
    if(self == NULL) {
        fprintf(stderr, "Runtime Error at line %d: %s is not defined...\n", line, key);
        exit(EXIT_FAILURE);
    }
    HashEntry result = dict_get(self->table, key);
    if(result.status == OCCUPIED) return result.value;
    return env_get(self->outer, key, line);
}

/**
 * グローバルから現在のスコープまで、識別子が定義されているか判定し、真偽を返します。
 */
bool env_exists(Environment* self, const char* key)
{
    if(self == NULL) return false;
    HashEntry result = dict_get(self->table, key);
    if(result.status == OCCUPIED) return true;
    return env_exists(self->outer, key);
}

/**
 * メモリ解放を行います。
 */
void env_free(Environment* self)
{
    dict_free(self->table);
    free(self);
}
