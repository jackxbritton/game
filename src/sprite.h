#ifndef SPRITE_H
#define SPRITE_H

#include <stddef.h>

struct Sprite {
    float buffer[24];
};
typedef struct Sprite Sprite;

void sprite_init(Sprite *sprite, float x1, float y1,
                                 float x2, float y2,
                                 float u1, float v1,
                                 float u2, float v2);

struct SpriteBatch {
    Sprite *sprites;
    size_t sprites_allocated,
           sprites_count;
};
typedef struct SpriteBatch SpriteBatch;

void sprite_batch_init(SpriteBatch *sprite_batch);
void sprite_batch_destroy(SpriteBatch *sprite_batch);

void sprite_batch_add(SpriteBatch *sprite_batch, Sprite *sprite);
void sprite_batch_clear(SpriteBatch *sprite_batch);

#endif
