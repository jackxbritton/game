#include "array.h"
#include "misc.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void array_init(Array *array, size_t nmemb, size_t size) {

    assert(array != NULL);
    assert(nmemb > 0);
    assert(size > 0);

    array->buffer = malloc(nmemb * size);
    if (array->buffer == NULL) {
        DEBUG("malloc failed.");
        return;
    }

    array->count     = 0;
    array->allocated = nmemb;
    array->slot_size = size;
}

void array_destroy(Array *array) {
    assert(array != NULL);
    free(array->buffer);
}

void array_resize(Array *array, size_t size) {
    assert(array != NULL);
    assert(size > 0);

    array->allocated = size;
    errno = 0;
    array->buffer = realloc(array->buffer, array->allocated*array->slot_size);
    if (errno == ENOMEM) {
        DEBUG("realloc failed.");
        return;
    }

    if (array->count > array->allocated) array->count = array->allocated;

}

void array_clone(Array *dest, Array *src) {
    assert(dest != NULL);
    assert(src != NULL);
    array_init(dest, src->allocated, src->slot_size);
    memcpy(dest->buffer, src->buffer, src->count*src->slot_size);
    dest->count = src->count;
}

void array_copy(Array *dest, Array *src) {
    assert(dest != NULL);
    assert(src != NULL);
    assert(src->slot_size == dest->slot_size);
    if (dest->allocated < src->count) array_resize(dest, src->count);
    memcpy(dest->buffer, src->buffer, src->count*src->slot_size);
    dest->count = src->count;
}

void array_add(Array *array, void *item) {

    assert(array != NULL);

    // Resize array?
    if (array->count == array->allocated) {
        array->allocated *= 2;
        errno = 0;
        array->buffer = realloc(array->buffer, array->allocated*array->slot_size);
        if (errno == ENOMEM) {
            DEBUG("realloc failed.");
            return;
        }
    }

    if (item != NULL) memcpy(&array->buffer[array->count*array->slot_size], item, array->slot_size);
    array->count++;

}

void array_clear(Array *array) {
    assert(array != NULL);
    array->count = 0;
}

void *array_get(Array *array, int index) {
    assert(array != NULL);
    assert(index >= 0 && index < array->count);
    return &array->buffer[index * array->slot_size];
}
