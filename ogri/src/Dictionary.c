#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#include "Dictionary.h"
#include "List.h"
#include "Object.h"

#define MAGIC_NUMBER 9981
#define STEP 3

/**
 * エラー文のヘルパー関数です。
 */
static void error(const char* string) {
    fprintf(stderr, "%s\n", string);
    exit(EXIT_FAILURE);
}

/**
 * ハッシュ値を計算します。
 */
static unsigned long hash(const char* key)
{
    unsigned long hash = 9981 + seed;
    const char* tmp = key;
    int c;

    while((c = *tmp++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

/**
 * 辞書をリサイズし、リハッシュします。
 */
static void resize(Dictionary* self, int new_capacity)
{
    List* old_entries = self->entries;
    int old_capacity = self->capacity;

    self->capacity = new_capacity;
    self->count = 0;
    self->entries = newList(HashEntry);
    LIST_ERROR rsvErr = reserve(self->entries, self->capacity);
    if(rsvErr != LIST_OK) error("Runtime Error: Failed to resize a Dictionary.\n");
        
    HashEntry empty = { .key = NULL, .value = NULL, .status = UNUSED};
    for(int index = 0; index < self->capacity; index++) {
        LIST_ERROR addErr = add(self->entries, &empty);
        if(addErr != LIST_OK) error("Runtime Error: Failed to get Entry.\n");
    }
    for(int index = 0; index < old_capacity; index++) {
        HashEntry entry;
        LIST_ERROR getErr = getAt(old_entries, index, HashEntry, &entry);
        if(getErr != LIST_OK) error("Runtime Error: Failed to get Entry.\n");
        
        if(entry.status == OCCUPIED) {
            dict_set(self, entry.key, entry.value);
            free((void*)entry.key);
        }
    }
    dList(old_entries);
}

/**
 * コンストラクタです。
 */
Dictionary* newDict(int init_capacity)
{
    Dictionary* self = malloc(sizeof(Dictionary));
    if(self == NULL) error("Runtime Error: Failed to make Dictionary.\n");
    self->capacity = init_capacity;
    self->count = 0;
    self->max_load_factor = (double)self->count / self->capacity;
    self->entries = newList(HashEntry);
    LIST_ERROR rsvErr = reserve(self->entries, self->capacity);
    if(rsvErr != LIST_OK) error("Runtime Error: Failed to reserve a Dictionary.\n");

    HashEntry empty = { .key = NULL, .value = NULL, .status = UNUSED};
    for(int index = 0; index < self->capacity; index++) {
        LIST_ERROR addErr = add(self->entries, &empty);
        if(addErr != LIST_OK) error("Runtime Error: Failed to initialize a Dictionary.\n");
    }
    return self;
}

/**
 * 辞書に登録します。
 */
bool dict_set(Dictionary* self, const char* key, Object* value)
{
    if(self == NULL) error("Runtime Error: Dictionary is Null.\n");

    if(self->max_load_factor > 0.7)
        resize(self, self->capacity * 2);

    unsigned long hash_value = hash(key);
    long index =  (long) (hash_value % (unsigned long)self->capacity);
    while(1) {
        HashEntry entry;
        LIST_ERROR getErr = getAt(self->entries, index, HashEntry, &entry);
        if(getErr != LIST_OK) error("Runtime Error: Failed to get Entry");

        if(entry.status == UNUSED || entry.status == DELETED) {
            entry.key = strdup(key);
            entry.value = value;
            entry.status = OCCUPIED;
            LIST_ERROR setErr = setAt(self->entries, index, HashEntry, &entry);
            if(setErr != LIST_OK) error("Runtime Error: Failed to set Entry");
            self->count++;
            self->max_load_factor = (double)self->count / self->capacity;
            return false;
        }
        if(strcmp(entry.key, key) == 0) {
            entry.value = obj_copy(value);
            LIST_ERROR setErr = setAt(self->entries, index, HashEntry, &entry);
            if(setErr != LIST_OK) error("Runtime Error: Failed to set Entry.");
            return true;
        }
        index = (index+STEP) % self->capacity;
    }
}

/**
 * 辞書からentryを得ます。
 */
HashEntry dict_get(Dictionary* self, const char* key)
{
    unsigned long hash_value = hash(key);
    long index =  (long) (hash_value % (unsigned long)self->capacity);
    int start_index = index;
    while(1) {
        HashEntry entry;
        LIST_ERROR getErr = getAt(self->entries, index, HashEntry, &entry);
        if(getErr != LIST_OK) error("Runtime Error: Failed to get Entry.");

        if(entry.status == UNUSED) return entry;

        if(entry.status == OCCUPIED && strcmp(key, entry.key) == 0) return entry;

        index = (index + STEP) % self->capacity;

        if(index == start_index) {
            HashEntry none = { .status = UNUSED };
            return none;
        }
    }
}

/**
 * 辞書のメモリを解放します。
 */
void dict_free(Dictionary* self) 
{
    if(self == NULL) return;
    for(int index = 0; index < self->capacity; index++) {
        HashEntry entry;
        getAt(self->entries, index, HashEntry, &entry);
        if(entry.status == OCCUPIED) {
            free((void*)entry.key);
        }
    }
    dList(self->entries);
    free(self);
}
