#ifndef SPRITE_H
#define SPRITE_H

#include <stddef.h>
#include <GL/glew.h>
#include "array.h"

struct Sprite {
    float buffer[24];
};
typedef struct Sprite Sprite;

void sprite_init(Sprite *sprite, float  x, float  y,
                                 float  w, float  h,
                                 float u1, float v1,
                                 float u2, float v2);

struct SpriteBatch {
    GLuint vao, vbo;
    Array sprites;
};
typedef struct SpriteBatch SpriteBatch;

void sprite_batch_init(SpriteBatch *sprite_batch, size_t size);
void sprite_batch_destroy(SpriteBatch *sprite_batch);

void sprite_batch_add(SpriteBatch *sprite_batch, Sprite *sprite);
void sprite_batch_clear(SpriteBatch *sprite_batch);

void sprite_batch_update_vbo(SpriteBatch *sprite_batch);

#endif
