/**
 * スコープとなります。
 */
#ifndef __ENVIRONMENT_H__
#define __ENVIRONMENT_H__

#include <stdbool.h>

typedef struct object Object;
typedef struct dictionary Dictionary;

typedef struct environment Environment;

struct environment {
    Dictionary* table;
    Environment* outer;
};

Environment* newEnv(Environment*);
void env_set(Environment*, const char*, Object*);
void env_define(Environment*, const char*, Object*);
void env_assign(Environment*, const char*, Object*, int);
Object* env_get(Environment*, const char*, int);
bool env_exists(Environment*, const char*);
void env_free(Environment*);


#endif /* __ENVIRONMENT_H__ */
