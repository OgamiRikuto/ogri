#include <stdio.h>
#include <stdlib.h>
#include "List.h"
#include "Object.h"
#include "Iterator.h"

/**
 * コンストラクタです。
 */
Iterator* newIterator(Object* list)
{
    Iterator* self = malloc(sizeof(Iterator));
    if(self == NULL) {
        fprintf(stderr, "Runtime Error: cannot make iterator...\n");
        exit(EXIT_FAILURE);
    }
    self->list = list;
    self->current = -1;
    return self;
}

/**
 * リストの次の値があるかどうかその真偽を返します。
 */
bool has_next(Iterator* self)
{
    if(self == NULL || self->list == NULL) return false;
   
    int size = getSize(self->list->list);
    return size > (self->current + 1);
}

/**
 * リストの次の値を計算して返します。
 */
Object* next(Iterator* self)
{
    if(!has_next(self)) {
        fprintf(stderr, "Runtime Error: Iterator has no next...\n");
        exit(EXIT_FAILURE);
    }
    self->current++;
    Object* content;
    LIST_ERROR getErr = getAt(self->list->list, self->current, Object*, &content);
    if(getErr != LIST_OK || content == NULL) {
        fprintf(stderr, "Runtime Error: Cannot get next in iterator...\n");
        exit(EXIT_FAILURE);
    }
    return content;
}
