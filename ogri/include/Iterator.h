/**
 * repeat文用のイテレーターです。
 */
#ifndef __ITERATOR_H__
#define __ITERATOR_H__

#include <stdbool.h>

typedef struct object Object;

typedef struct iterator Iterator;

struct iterator {
    Object* list;
    int current;
};

Iterator* newIterator(Object*);
bool has_next(Iterator*);
Object* next(Iterator*);

#endif /* __ITERATOR_H__ */
