#include "utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

void * safe_alloc(size_t size){
    void * pointer = malloc(size);
    if(!pointer){
        perror("Malloc error.\n");
        exit(EXIT_FAILURE);
    }
    return pointer;
}

void * safe_realloc(void * ptr, size_t size){
    ptr = realloc(ptr, size);
    if(!ptr){
        perror("Realloc error.\n");
        exit(EXIT_FAILURE);
    }
    return ptr;
}

void append_char(char * str, char chr){
    size_t len = strlen(str);
    str[len] = chr;
    str[len + 1] = '\0';
}