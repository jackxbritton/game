#include "font.h"
#include "misc.h"
#include <assert.h>

void font_init(Font *font, FT_Library *ft, const char *filename) {

    const int start = ' ',
                end = 'Z';

    assert(font != NULL);
    assert(ft != NULL);
    assert(filename != NULL);

    font->ft = ft;

    // Load the face.
    int error = FT_New_Face(*ft, filename, 0, &font->face);
    if (error == FT_Err_Unknown_File_Format) {
        DEBUG("Uknown font file format: '%s'.", filename);
        return;
    }
    if (error) {
        DEBUG("FT_New_Face failed for font '%s'.", filename);
        return;
    }

    const int size = 12;
    FT_Set_Pixel_Sizes(font->face, 0, size);

    // First, find the width and height of the texture map.
    // We're storing all the chars in one long row.

    int width = 0, height = 0;

    for (char c = start; c <= end; c++) {

        if (FT_Load_Char(font->face, c, FT_LOAD_RENDER)) {
            DEBUG("FT_Load_Char failed for char '%c'.", c);
            continue;
        }

        FT_GlyphSlot g = font->face->glyph;

        // Update width and height.
        width += g->bitmap.width;
        if (height < g->bitmap.rows) height = g->bitmap.rows;
    }

    // Allocate the texture.
    glGenTextures(1, &font->gl_texture);
    glBindTexture(GL_TEXTURE_2D, font->gl_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);

    // Run through the chars again and blit to the texture.
    int x = 0;
    for (char c = start; c <= end; c++) {

        // Load the character.
        if (FT_Load_Char(font->face, c, FT_LOAD_RENDER)) {
            DEBUG("FT_Load_Char failed for char '%c'.", c);
            continue;
        }

        FT_GlyphSlot g = font->face->glyph;

        // Copy the bitmap.
        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        x, 0,
                        g->bitmap.width, g->bitmap.rows,
                        GL_RED, GL_UNSIGNED_BYTE,
                        g->bitmap.buffer);

        x += g->bitmap.width;
    }

    // Fill glyph_info.

    font->glyph_info = malloc((end - start + 1) * sizeof(GlyphInfo));
    if (font->glyph_info == NULL) {
        DEBUG("malloc failed.");
        return;
    }

    for (char c = start; c <= end; c++) {

        FT_GlyphSlot g = font->face->glyph;
        GlyphInfo *gi = &font->glyph_info[c - start];

        gi->advance_x = g->advance.x;
        gi->advance_y = g->advance.y;

        gi->u1 = (float) x / width;
        gi->u2 = gi->u1 + (float) g->bitmap.width / width;
        gi->v1 = 1.0 - (float) g->bitmap.rows / height;
        gi->v2 = 1.0;

        gi->metrics = g->metrics;

    }

    // Harfbuzz.
    font->hb_font = hb_ft_font_create(font->face, NULL);
    if (font->hb_font == NULL) {
        DEBUG("hb_ft_font_create failed.");
        return;
    }
    font->hb_face = hb_ft_face_create(font->face, NULL);
    if (font->hb_face == NULL) {
        DEBUG("hb_ft_face_create failed.");
        return;
    }

}

void font_destroy(Font *font) {
    assert(font != NULL);
    free(font->glyph_info);
}
