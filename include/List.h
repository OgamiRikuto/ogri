/**
 * 動的リストです。
 */
#ifndef __LIST_H__
#define __LIST_H__

#include<stdlib.h>
#include<string.h>

typedef enum {
    LIST_OK = 0,                        // 正常終了
    LIST_ALLOCATE_FAILIER,              // メモリ確保失敗
    LIST_INDEX_OUT_OF_RANGE_IN_SET,     // 範囲外指定 (set内)  
    LIST_INDEX_OUT_OF_RANGE_IN_INSERT,  // 範囲外指定 (insert内)
    LIST_INDEX_OUT_OF_RANGE_IN_REMOVE,  // 範囲外指定 (remove内)
    LIST_INDEX_OUT_OF_RANGE_IN_GET,     // 範囲外指定 (get内)
    LIST_TYPE_MISMATCH,                 // 型不一致
    LIST_NULL,                          // リストのポインタがNULL
    LIST_DATA_NULL,                     // リスト内データがNULL
    LIST_REVERSE_FAILIER,               // リバース不可能なサイズ
    LIST_CAPACITY_ZERO,                 // 指定キャパシティがゼロ
    LIST_ERR_MAX
} LIST_ERROR;

typedef struct _list List;

List* new_list(size_t, const char*);
void dList(List*);
LIST_ERROR add(List*, void*);
LIST_ERROR set(List*, int, const char*, void*);
LIST_ERROR insert(List*, int, const char*, void*);
LIST_ERROR removeAt(List*, int);
LIST_ERROR get(List*, int, const char*, void*);
LIST_ERROR reserve(List*, int);
LIST_ERROR clone(List*, List*);
LIST_ERROR reverse(List*);
int getSize(List*);
int getCapacity(List*);

void printListErr(LIST_ERROR);
void plerr(LIST_ERROR, const char*);

#define setAt(LIST_PTR, INDEX, TYPE, DATA) set(LIST_PTR, INDEX, #TYPE, DATA)
#define insertAt(LIST_PTR, INDEX, TYPE, DATA) insert(LIST_PTR, INDEX, #TYPE, DATA)
#define getAt(LIST_PTR, INDEX, TYPE, VAR) get(LIST_PTR, INDEX, #TYPE, VAR)
#define newList(TYPE) (new_list(sizeof(TYPE), #TYPE))

#endif /* __LIST_H__ */
