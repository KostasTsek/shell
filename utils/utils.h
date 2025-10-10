#ifndef UTILS_H
#define UTILS_H

#define LINE_MAX 2048

#include <stdlib.h>

void * safe_alloc(size_t size);

void * safe_realloc(void * ptr, size_t size);

void append_char(char * str, char chr);

#endif