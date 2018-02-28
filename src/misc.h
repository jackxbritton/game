#ifndef MISC_H
#define MISC_H

#include <stdio.h>
#include <errno.h>

#define DEBUG(fmt, ...) \
        fprintf(stderr, "%s:%s:%d: " fmt "\n", __FILE__, __func__, __LINE__, ##__VA_ARGS__)

char *load_entire_file(const char *filename);
char *copy_string(const char *str);

//void *xmalloc(size_t size);
//void *xcalloc(size_t nmemb, size_t size);
//void *xrealloc(void *ptr, size_t size);

// Array.

struct Array {
    unsigned char *buffer;
    size_t count,
           allocated,
           slot_size;
};
typedef struct Array Array;

void array_init(Array *array, size_t nmemb, size_t size);
void array_destroy(Array *array);

void array_add(Array *array, void *item);
void array_clear(Array *array);

void *array_get(Array *array, int index);

#endif
