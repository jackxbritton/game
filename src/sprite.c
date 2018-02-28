#include "sprite.h"
#include <assert.h>
#include <stddef.h>

void sprite_init(Sprite *sprite, float x1, float y1,
                                 float x2, float y2,
                                 float u1, float v1,
                                 float u2, float v2) {
    assert(sprite != NULL);
    float *b = sprite->buffer;
    b[ 0] = x1;  b[ 1] = y1;  b[ 2] = u1;  b[ 3] = v1;
    b[ 4] = x1;  b[ 5] = y2;  b[ 6] = u1;  b[ 7] = v2;
    b[ 8] = x2;  b[ 9] = y2;  b[10] = u2;  b[11] = v2;

    b[12] = x1;  b[13] = y1;  b[14] = u1;  b[15] = v1;
    b[16] = x2;  b[17] = y1;  b[18] = u2;  b[19] = v1;
    b[20] = x2;  b[21] = y2;  b[22] = u2;  b[23] = v2;
}
