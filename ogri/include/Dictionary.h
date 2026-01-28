/**
 * ハッシュテーブルである辞書です。
 */
#ifndef __DICTIONARY_H__
#define __DICTIONARY_H__

#include <stdbool.h>

typedef struct _list List;
typedef struct object Object;

typedef struct hashentry HashEntry;
typedef struct dictionary Dictionary;

typedef enum {
    UNUSED,
    OCCUPIED,
    DELETED
} EntryStatus;

struct hashentry {
    const char* key;
    Object* value;
    EntryStatus status;
};

struct dictionary {
    List* entries;
    int capacity;
    int count;
    double max_load_factor;
};

Dictionary* newDict(int);
bool dict_set(Dictionary*, const char*, Object*);
HashEntry dict_get(Dictionary*, const char*);
void dict_free(Dictionary*);

extern long seed;

#endif /* __DICTIONARY_H__ */
