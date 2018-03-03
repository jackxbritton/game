#ifndef ARRAY_H
#define ARRAY_H

#include <stddef.h>

struct Array {
    unsigned char *buffer;
    size_t count,
           allocated,
           slot_size;
};
typedef struct Array Array;

void array_init(Array *array, size_t nmemb, size_t size);
void array_destroy(Array *array);

void array_copy(Array *dest, Array *src);

void array_add(Array *array, void *item);
void array_clear(Array *array);

void *array_get(Array *array, int index);

#endif
