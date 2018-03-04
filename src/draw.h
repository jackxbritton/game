#ifndef DRAW_H
#define DRAW_H

#include "shader_program.h"
#include "text.h"
#include "texture.h"
#include "sprite.h"
#include "catalog.h"

struct DrawContext {

    float aspect;
    int width,
        height;
    float hdpi,
          vdpi;

    ShaderProgram text_shader,
                  quad_shader;
    GLint u_texture,
          u_color,
          u_transform;

    GLuint bound_program,
           bound_texture;

    Catalog *catalog;

    GLuint circle_texture; // TODO

};
typedef struct DrawContext DrawContext;

void draw_context_init(DrawContext *dc, Catalog *catalog, float aspect, int width, int height, float hdpi, float vdpi);
void draw_context_destroy(DrawContext *dc);

void draw_resize(DrawContext *dc, int width, int height);

void draw_clear(DrawContext *dc);

// Text rendering.

enum TextAlignment {
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT
};
typedef enum TextAlignment TextAlignment;

void draw_set_color(DrawContext *dc, float r, float g, float b, float a);
void draw_string(DrawContext *dc, Font *font, const char *str, float x, float y, TextAlignment alignment);
void draw_text(DrawContext *dc, Text *text, float x, float y, TextAlignment alignment);

void draw_sprite(DrawContext *dc, Sprite *sprite, Texture *texture);

void draw_sprite_batch(DrawContext *dc, SpriteBatch *sprite_batch, Texture *texture, float x, float y);

void draw_circle(DrawContext *dc, float x, float y, float r);

#endif
