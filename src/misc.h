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

#endif
