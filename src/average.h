#ifndef AVERAGE_H
#define AVERAGE_H

struct Average {
    float *buffer;
    int n,
        current;
};
typedef struct Average Average;

void average_init(Average *a, int n);
void average_destroy(Average *a, int n);

void average_add(Average *a, float f);
float average_calc(Average *a);

#endif
