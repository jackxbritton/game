#include "average.h"
#include "misc.h"
#include <assert.h>
#include <stdlib.h>

void average_init(Average *a, int n) {

    assert(a != NULL);
    assert(n > 0);

    a->buffer = calloc(n, sizeof(float));
    if (a->buffer == NULL) {
        DEBUG("malloc failed.");
        return;
    }

    a->n = n;
    a->current = 0;

}

void average_destroy(Average *a, int n) {
    assert(a != NULL);
    free(a->buffer);
}

void average_add(Average *a, float f) {
    assert(a != NULL);
    a->buffer[a->current] = f;
    a->current = (a->current + 1) % a->n;
    DEBUG("%d", a->current);
}

float average_calc(Average *a) {
    assert(a != NULL);
    float sum = 0.0f;
    for (int i = 0; i < a->n; i++) {
        sum += a->buffer[i];
    }
    return sum / a->n;
}
