#ifndef SPRITE_H
#define SPRITE_H

struct Sprite {
    float buffer[24];
};
typedef struct Sprite Sprite;

void sprite_init(Sprite *sprite, float x1, float y1,
                                 float x2, float y2,
                                 float u1, float v1,
                                 float u2, float v2);


#endif
