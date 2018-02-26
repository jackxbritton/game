#ifndef FONT_H
#define FONT_H

#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>

struct GlyphInfo {
    float advance_x,
          advance_y;
    float u1, v1,
          u2, v2;
    FT_Glyph_Metrics metrics;
};
typedef struct GlyphInfo GlyphInfo;

struct Font {
    FT_Library *ft;
    FT_Face face;
    GLuint gl_texture;
    GlyphInfo *glyph_info;

    hb_font_t   *hb_font; // TODO
    hb_face_t   *hb_face;
};
typedef struct Font Font;

void font_init(Font *font, FT_Library *ft, const char *filename);
void font_destroy(Font *font);

#endif
