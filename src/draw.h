#ifndef DRAW_H
#define DRAW_H

#include "shader_program.h"
#include "font.h"
#include "catalog.h"

struct DrawContext {

    float aspect;
    int width,
        height;

    ShaderProgram text_shader;
    GLint u_texture,
          u_color;

    FT_Library ft;
    Font font;

    Catalog catalog;

};
typedef struct DrawContext DrawContext;

void draw_context_init(DrawContext *dc, float aspect, float hdpi, float vdpi);
void draw_context_destroy(DrawContext *dc);

void draw_resize(DrawContext *dc, int width, int height);

void draw_clear(DrawContext *dc);

void draw_string(DrawContext *dc, const char *str);

#endif
