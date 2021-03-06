#ifndef FONT_H
#define FONT_H

#include <GL/glew.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <harfbuzz/hb.h>
#include <harfbuzz/hb-ft.h>
#include "array.h"

struct GlyphInfo {
    float u1, v1,
          u2, v2;
    FT_Glyph_Metrics metrics;
};
typedef struct GlyphInfo GlyphInfo;

// Font.

struct Font {

    FT_Library *ft;
    FT_Face face;
    GLuint gl_texture;
    GlyphInfo *glyph_info;

    char start, end;
    int size;

    hb_font_t *hb_font;
    hb_face_t *hb_face;
    hb_buffer_t *hb_buffer;

    //SpriteBatch sprite_batch;
    Array sprite_batch;

};
typedef struct Font Font;

void font_init(Font *font, FT_Library *ft, const char *filename, int point_size, float hdpi, float vdpi);
void font_destroy(Font *font);

int font_contains_char(Font *font, char c);

#endif
