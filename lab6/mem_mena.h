#ifndef MEM_MENA_H
#define MEM_MENA_H

#include <stdlib.h>

#define MEM_SUCCESS         0
#define MEM_ERR_MALLOC      1
#define MEM_ERR_INVALID_PTR 2
#define MEM_ERR_FREE        3
#define MEM_ERR_CALLOC      4
#define MEM_ERR_REALLOC     5

extern int mem_errno;

void *mem_allock(void *ptr, unsigned int size);
int mem_free(void *ptr);

#endif
