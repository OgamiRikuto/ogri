#include "../include/List.h"
#include<stdio.h>

#define LIST_DEFAULT_CAPACITY 4

typedef struct _list{
    void *data;
    int size;
    int capacity;
    size_t data_size;
    const char* type_name;
}List;

// コンストラクタ
List* new_list(size_t data_size, const char* type)
{
    List* this = NULL;

    this = (List*)malloc(sizeof(List));
    if(this==NULL) return NULL;

    this->size = 0;
    this->capacity = LIST_DEFAULT_CAPACITY;
    this->data = malloc(data_size * this->capacity);
    if(this->data == NULL) return NULL;
    this->data_size = data_size;
    this->type_name = type;

    return this;
}

// デストラクタ
void dList(List* self)
{
    free(self->data);
    free(self);
}

// 末尾にデータを追加する
LIST_ERROR add(List* self, void* data)
{
    if(self == NULL) return LIST_NULL;

    if(self->size >= self->capacity) {
        LIST_ERROR err_code = reserve(self, self->capacity * 2);
        if(err_code != LIST_OK) {
            fprintf(stderr, "in add...\n");
            return LIST_ALLOCATE_FAILIER;
        }
    }

    memcpy((char*)self->data + self->data_size * self->size, data, self->data_size);
    self->size++;
    
    return LIST_OK;
}

// 指定した場所のデータを変更する
LIST_ERROR set(List* self, int index, const char* type, void* data)
{
    if(self == NULL) return LIST_NULL;
    if(index < 0 || index >= self->size) return LIST_INDEX_OUT_OF_RANGE_IN_SET;
    if(strcmp(type, self->type_name) != 0) return LIST_TYPE_MISMATCH;

    memcpy((char*)self->data + self->data_size * index, data, self->data_size);
    return LIST_OK;
}

// データを挿入する
LIST_ERROR insert(List* self, int index, const char* type, void* data)
{
    if(self == NULL) return LIST_NULL;
    if(index < 0 || index > self->size) return LIST_INDEX_OUT_OF_RANGE_IN_INSERT;
    if(self->size >= self->capacity) {
        LIST_ERROR err_code = reserve(self, self->capacity * 2);
        if(err_code != LIST_OK) {
            fprintf(stderr, "in insert...\n");
            return LIST_ALLOCATE_FAILIER;
        }
    }

    char* base = (char*) self->data;
    memmove(
        base + self->data_size * (index+1),
        base + self->data_size * index,
        self->data_size * (self->size - index)
    );
    memcpy(base + self->data_size * index, data, self->data_size);
    self->size++;
    return LIST_OK;
}

// 指定した場所のデータを削除する
LIST_ERROR removeAt(List* self, int index)
{
    if(self == NULL) return LIST_NULL;
    if(index < 0 || index >= self->size) return LIST_INDEX_OUT_OF_RANGE_IN_REMOVE;
    char* base = (char*)self->data;
    memmove(
        base + self->data_size * index,
        base + self->data_size * (index+1),
        self->data_size * (self->size - index - 1)
    );
    self->size--;
    return LIST_OK;
}

// 指定した場所のデータを得る
LIST_ERROR get(List* self, int index, const char* type, void* out)
{
    if(self == NULL) return LIST_NULL;
    if(index < 0 || index >= self->size) {
        return LIST_INDEX_OUT_OF_RANGE_IN_GET;
    }
    if(strcmp(self->type_name, type) != 0) return LIST_TYPE_MISMATCH;

    memcpy(out, (char*)self->data + self->data_size * index, self->data_size);
    return LIST_OK;
}

// あらかじめ指定したデータ数の容量を確保する
LIST_ERROR reserve(List* self, int new_capacity)
{
    if(self == NULL) return LIST_NULL;
    if(new_capacity <= 0) return LIST_CAPACITY_ZERO;
    if(new_capacity <= self->capacity) return LIST_OK;

    void* tmp = realloc(self->data, self->data_size * new_capacity);
    if(tmp == NULL) return LIST_ALLOCATE_FAILIER;

    self->data = tmp;
    self->capacity = new_capacity;

    return LIST_OK;
}


// リストのコピーを作成する
LIST_ERROR clone(List* dest, List* src)
{
    if(dest == NULL || src == NULL) return LIST_NULL; 
    if(src->data == NULL) return LIST_DATA_NULL;
    if(dest->type_name == NULL || src->type_name == NULL) return LIST_NULL;
    if(strcmp(dest->type_name, src->type_name) != 0) return LIST_TYPE_MISMATCH;
    if(dest->data_size != src->data_size) return LIST_TYPE_MISMATCH;

    LIST_ERROR err_code = reserve(dest, src->capacity);
    if(err_code != LIST_OK) {
        fprintf(stderr, "in clone...\n");
        return LIST_ALLOCATE_FAILIER;
    }

    dest->size = src->size;
    dest->capacity = src->capacity;

    memcpy(dest->data, src->data, dest->size * dest->data_size);
    return LIST_OK;
}

// リストの中身を逆順にする
LIST_ERROR reverse(List* self)
{
    if(self == NULL) return LIST_NULL;
    if(self->size < 1) return LIST_REVERSE_FAILIER;

    if(self->size == 1) return LIST_OK;

    char* base = (char*) self->data;
    size_t dataSize = self->data_size;
    int left = 0;
    int right = self->size - 1;

    void* tmp = malloc(dataSize);
    if(tmp == NULL) {
        fprintf(stderr, "in reverse...\n");
        return LIST_ALLOCATE_FAILIER;
    }

    while(left < right) {
        void* left_ptr = base + dataSize * left;
        void* right_ptr = base + dataSize * right;

        memcpy(tmp, left_ptr, dataSize);
        memcpy(left_ptr, right_ptr, dataSize);
        memcpy(right_ptr, tmp, dataSize);
        left++;
        right--;
    }
    free(tmp);
    return LIST_OK;
}

// リストのサイズを得る
int getSize(List *self)
{
    return self->size;
}

// リストのキャパシティを得る
int getCapacity(List* self)
{
    return self->capacity;
}

void printListErr(LIST_ERROR err)
{
    static const char* errMessage[LIST_ERR_MAX] = {
        "",
        "Error: Memory allocation failed.\n",
        "Error: Index out of range in set.\n",
        "Error: Index out of range in insert.\n",
        "Error: Index out of range in remove.\n",
        "Error: Index out of range in get.\n",
        "Error: Type mismatch.\n",
        "Error: List is NULL.\n",
        "Error: List data is NULL.\n",
        "Error: List reverse failed.\n",
        "Error: Capacity is less than zero.\n"
    };

    if(err < 0 || err >= LIST_ERR_MAX) {
        fprintf(stderr, "ERROR: Unkown list error.\n");
        return;
    }

    fprintf(stderr, "%s", errMessage[err]);
}

void plerr(LIST_ERROR err, const char* str)
{
    if(err == LIST_OK) return;
    fprintf(stderr, "%s\n", str);
    printListErr(err);
}
