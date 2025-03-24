#include <stdio.h>
#include <string.h>
#include "mem_mena.h"
int main(void) {
char *str = (char *)mem_allock(NULL, 100);
    if(!str){
        fprintf(stderr, "mem_allock erro: %d\n", mem_errno);
        return 1;}
snprintf(str, 100, "start, mem menager!!!");
    printf("allocated str: %s\n", str);

str = (char *)mem_allock(str, 200);
    if (!str) {
        fprintf(stderr, "mem_allock (realloc) error: %d\n", mem_errno);
        return 1;}
strcat(str, "wiecej po relo");
    printf("reallocated str: %s\n", str);
if (mem_free(str) != 0) {
        fprintf(stderr, "mem_free error: %d\n", mem_errno);
        return 1;
    }printf("zwolniono pamiec!!!.\n");
if (mem_free(str) != 0) {
        printf("nie pozwala ponownie zwolnic- git!!!! erro: %d).\n", mem_errno);
    }return 0;}
