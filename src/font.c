#include "font.h"
#include "misc.h"
#include <assert.h>

void font_init(Font *font, FT_Library *ft, const char *filename, int point_size, int hdpi, int vdpi) {

    assert(font != NULL);
    assert(ft != NULL);
    assert(filename != NULL);

    font->ft = ft;
    font->start = ' ';
    font->end = 'z';

    // Load the face.
    int error = FT_New_Face(*ft, filename, 0, &font->face);
    if (error == FT_Err_Unknown_File_Format) {
        DEBUG("Uknown font file format: '%s'.", filename);
        return;
    }
    if (error) {
        DEBUG("FT_New_Face failed for filename='%s'.", filename);
        return;
    }

    error = FT_Set_Char_Size(font->face, 0, point_size*64, hdpi, vdpi);
    if (error) {
        DEBUG("FT_Set_Char_Size failed for filename='%s', point_size=%d, hdpi=%d, vdpi=%d.", filename, point_size, hdpi, vdpi);
        return;
    }

    // First, find the width and height of the texture map.
    // We're storing all the chars in one long row.

    int width = 0,
        height = 0;

    for (char c = font->start; c <= font->end; c++) {

        if (FT_Load_Char(font->face, c, FT_LOAD_RENDER)) {
            DEBUG("FT_Load_Char failed for char '%c'.", c);
            continue;
        }

        FT_GlyphSlot g = font->face->glyph;

        if (g->bitmap.width == 0 && g->bitmap.rows == 0) continue;

        // Update width and height.
        width += g->bitmap.width;
        if (height < g->bitmap.rows) height = g->bitmap.rows;
    }

    // Allocate the texture.
    glGenTextures(1, &font->gl_texture);
    glBindTexture(GL_TEXTURE_2D, font->gl_texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_R8, width, height);

    // Allocate glyph info.
    font->glyph_info = malloc((font->end - font->start + 1) * sizeof(GlyphInfo));
    if (font->glyph_info == NULL) {
        DEBUG("malloc failed.");
        return;
    }

    // Run through the chars again and blit to the texture.
    int x = 0;
    for (char c = font->start; c <= font->end; c++) {

        // Load the character.
        if (FT_Load_Char(font->face, c, FT_LOAD_RENDER)) {
            DEBUG("FT_Load_Char failed for char '%c'.", c);
            continue;
        }

        FT_GlyphSlot g = font->face->glyph;
        GlyphInfo *gi = &font->glyph_info[c - font->start];

        gi->metrics = g->metrics;

        if (g->bitmap.width == 0 && g->bitmap.rows == 0) continue;

        // Copy the bitmap.
        glTexSubImage2D(GL_TEXTURE_2D, 0,
                        x, 0,
                        g->bitmap.width, g->bitmap.rows,
                        GL_RED, GL_UNSIGNED_BYTE,
                        g->bitmap.buffer);

        // Fill glyph info.
        gi->u1 = (x + 0.5f) / width;
        gi->u2 = (x + g->bitmap.width - 0.5f) / width;
        gi->v1 = 1.0f - (g->bitmap.rows - 0.5f) / height;
        gi->v2 = 1.0f - 0.5f/height;

        x += g->bitmap.width;

    }

    // Harfbuzz.

    font->hb_font = hb_ft_font_create(font->face, NULL);
    if (font->hb_font == NULL) {
        DEBUG("hb_ft_font_create failed for filename='%s'.", filename);
        return;
    }

    font->hb_face = hb_ft_face_create(font->face, NULL);
    if (font->hb_face == NULL) {
        DEBUG("hb_ft_face_create failed for filename='%s'.", filename);
        return;
    }

    font->hb_buffer = hb_buffer_create();
    if (font->hb_buffer == NULL) {
        DEBUG("hb_buffer_create failed.");
        return;
    }

    sprite_batch_init(&font->sprite_batch);

}

void font_destroy(Font *font) {

    assert(font != NULL);

    free(font->glyph_info);

    //hb_ft_face_destroy(font->hb_face);
    //hb_ft_font_destroy(font->hb_font);
    hb_buffer_destroy(font->hb_buffer);

    sprite_batch_destroy(&font->sprite_batch);

    FT_Done_Face(font->face);
}

int font_contains_char(Font *font, char c) {
    if (c == '\n') return 1;
    if (c == '\t') return 1;
    if (c >= font->start && c <= font->end) return 1;
    DEBUG("Character '%c' is outside of glyph range '%c'-'%c'.", c, font->start, font->end);
    return 0;
}
