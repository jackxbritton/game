#ifndef DRAW_H
#define DRAW_H

#include "shader_program.h"
#include "font.h"
#include "catalog.h"

struct DrawContext {
    ShaderProgram text_shader;
    FT_Library ft;
    Font font;
    Catalog catalog;
    GLint u_texture,
          u_color;
};
typedef struct DrawContext DrawContext;

void draw_context_init(DrawContext *dc);
void draw_context_destroy(DrawContext *dc);

void draw_string(DrawContext *dc, const char *str);

#endif
