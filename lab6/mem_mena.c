#include "mem_manager.h"
#include <stdlib.h>
#include <string.h>

typedef struct MemoryBlock {
    void *pointer;
    unsigned int size;
    struct MemoryBlock *next;
} MemoryBlock;

static MemoryBlock *allocated_blocks = NULL;
int mem_errno = MEM_SUCCESS;

static void free_all_memory_blocks(void) {
    MemoryBlock *current = allocated_blocks;
    while (current) {
        MemoryBlock *next = current->next;
        free(current->pointer);
        free(current);
        current = next;
    }
    allocated_blocks = NULL;
}

static MemoryBlock *find_block(void *pointer) {
    MemoryBlock *current = allocated_blocks;
    while (current) {
        if (current->pointer == pointer)
            return current;
        current = current->next;
    }
    return NULL;
}

void *mem_allock(void *pointer, unsigned int size) {
    if (pointer == NULL) {
        void *new_pointer = calloc(1, size);
        if (!new_pointer) {
            mem_errno = MEM_ERROR_CALLOC;
            return NULL;
        }

        MemoryBlock *block = malloc(sizeof(MemoryBlock));
        if (!block) {
            free(new_pointer);
            mem_errno = MEM_ERROR_MALLOC;
            return NULL;
        }

        block->pointer = new_pointer;
        block->size = size;
        block->next = allocated_blocks;
        allocated_blocks = block;

        return new_pointer;
    }

    MemoryBlock *block = find_block(pointer);
    if (!block) {
        mem_errno = MEM_ERROR_INVALID;
        return NULL;
    }

    void *resized_pointer = realloc(pointer, size);
    if (!resized_pointer) {
        mem_errno = MEM_ERROR_REALLOC;
        return NULL;
    }

    if (size > block->size)
        memset((char *)resized_pointer + block->size, 0, size - block->size);

    block->pointer = resized_pointer;
    block->size = size;
    return resized_pointer;
}

int mem_free(void *pointer) {
    MemoryBlock *current = allocated_blocks, *previous = NULL;

    while (current) {
        if (current->pointer == pointer) {
            if (previous)
                previous->next = current->next;
            else
                allocated_blocks = current->next;

            free(current->pointer);
            free(current);
            return 0;
        }
        previous = current;
        current = current->next;
    }

    mem_errno = MEM_ERROR_INVALID;
    return -1;
}

__attribute__((constructor))
static void initialize_memory_manager() {
    atexit(free_all_memory_blocks);
}
