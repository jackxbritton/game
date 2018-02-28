#ifndef DRAW_H
#define DRAW_H

#include "shader_program.h"
#include "font.h"
#include "text.h"
#include "catalog.h"

struct DrawContext {

    float aspect;
    int width,
        height;

    ShaderProgram text_shader;
    GLint u_texture,
          u_color,
          u_transform;

    FT_Library ft;
    Font font;

    Catalog catalog;

};
typedef struct DrawContext DrawContext;

void draw_context_init(DrawContext *dc, float aspect, float hdpi, float vdpi);
void draw_context_destroy(DrawContext *dc);

void draw_resize(DrawContext *dc, int width, int height);

void draw_clear(DrawContext *dc);

enum TextAlignment {
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT
};
typedef enum TextAlignment TextAlignment;

void draw_string(DrawContext *dc, const char *str, float x, float y, TextAlignment alignment);
void draw_text(DrawContext *dc, Text *text, float x, float y, TextAlignment alignment);

#endif
