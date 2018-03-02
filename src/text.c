#include "text.h"
#include "misc.h"
#include <assert.h>
#include "sprite.h"

void text_init(Text *text, Font *font, const char *str, GLuint program) {

    assert(text != NULL);
    assert(font != NULL);
    assert(str != NULL);

    text->font = font;

    // Define sprite buffer.

    const int len = strlen(str);
    if (len == 0) {
        DEBUG("strlen(str) is 0.");
        return;
    }

    // Harfbuzz.

    hb_buffer_clear_contents(font->hb_buffer);
    hb_buffer_set_direction(font->hb_buffer, HB_DIRECTION_LTR);

    // Actually add the text and shape it.
    hb_buffer_add_utf8(font->hb_buffer, str, strlen(str), 0, strlen(str));
    hb_shape(font->hb_font, font->hb_buffer, NULL, 0);

    // Get the position data.
    unsigned int glyph_count;
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(font->hb_buffer, &glyph_count);

    // Fill the buffer.

    // TODO Check array->allocated, maybe we can skip a bunch of ifs (from array_add).
    // Pass the array as an argument, instead of using the one from font.

    Array *array = &font->sprite_batch;
    array_clear(array);

    float x = 0.0f,
          y = 0.0f;

    text->width = 0.0f;
    text->height = font->face->size->metrics.height/64;

    for (int i = 0; i < len; i++) {

        char c = str[i];
        if (!font_contains_char(font, c)) continue;

        if (c == '\n') {
            if (x > text->width) text->width = x;
            x = 0.0f;
            y -= font->face->size->metrics.height/64;
            text->height += font->face->size->metrics.height/64;
            continue;
        }

        if (c == '\t') c = ' ';

        const GlyphInfo *gi = &font->glyph_info[c-font->start];

        if (gi->metrics.width > 0 && gi->metrics.height > 0) {

            const float xi = x  + (glyph_pos[i].x_offset + gi->metrics.horiBearingX)/64;
            const float  w = gi->metrics.width/64;
            const float yi = y  - (gi->metrics.height - gi->metrics.horiBearingY - glyph_pos[i].y_offset)/64;
            const float  h = gi->metrics.height/64;

            Sprite sprite;
            sprite_init(&sprite, xi, yi, w, h, gi->u1, gi->v1, gi->u2, gi->v2);
            array_add(array, &sprite);
        }

        // TODO With some fonts, x_advance is incorrect.
        //      In particular this happens with DejaVuSerif after "fi".

        x += glyph_pos[i].x_advance/64;
        y -= glyph_pos[i].y_advance/64;

    }

    if (x > text->width) text->width = x;

    text->buffer_len = array->count * sizeof(Sprite);

    // Generate vbo.
    glGenVertexArrays(1, &text->vao);
    glBindVertexArray(text->vao);
    glGenBuffers(1, &text->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, text->vbo);
    glBufferData(GL_ARRAY_BUFFER, text->buffer_len, array->buffer, GL_DYNAMIC_DRAW);

    // Attrib pointers.
    glVertexAttribPointer(
        0,
        2,
        GL_FLOAT,
        GL_FALSE,
        4*sizeof(GLfloat),
        0
    );
    glEnableVertexAttribArray(0);
    glBindAttribLocation(program, 0, "point");
    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4*sizeof(GLfloat),
        (void *) (2*sizeof(GLfloat))
    );
    glEnableVertexAttribArray(1);
    glBindAttribLocation(program, 1, "uv");

}

void text_destroy(Text *text) {
    assert(text != NULL);
    glDeleteVertexArrays(1, &text->vbo);
    glDeleteBuffers(1, &text->vbo);
}
