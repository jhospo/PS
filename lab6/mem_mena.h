#ifndef MEM_MANA_H
#define MEM_MANA_H

#include <stdlib.h>

extern int mem_errno;

#define MEM_SUCCESS         0
#define MEM_ERR_MALLOC      1
#define MEM_ERR_INVALID_PTR 2
#define MEM_ERR_FREE        3
#define MEM_ERR_CALLOC      4
#define MEM_ERR_REALLOC     5

void *mem_allock(void *ptr, unsigned int size);
int mem_free(void *ptr);
void init_mem_manager(void);

#endif
