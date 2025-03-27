#include <stdio.h>
#include <string.h>
#include "mem_manager.h"

int main() {
    char *buffer = mem_allock(NULL, 100);
    if (!buffer) {
        printf("Allocation error: %d\n", mem_errno);
        return 1;
    }

    strcpy(buffer, "Simple memory manager test.");
    printf("Buffer: %s\n", buffer);

    buffer = mem_allock(buffer, 200);
    if (!buffer) {
        printf("Reallocation error: %d\n", mem_errno);
        return 1;
    }

    strcat(buffer, " Realloc worked!");
    printf("Updated buffer: %s\n", buffer);

    if (mem_free(buffer) != 0)
        printf("Free failed: %d\n", mem_errno);
    else
        printf("Memory freed successfully.\n");

    if (mem_free(buffer) != 0)
        printf("Second free blocked (correct): %d\n", mem_errno);

    return 0;
}
