#include "text.h"
#include "misc.h"
#include <assert.h>

void text_init(Text *text,
               Font *font,
               const char *str, float x, float y, TextAlignment alignment,
               GLuint program, int viewport_width, int viewport_height) {

    assert(text != NULL);
    assert(font != NULL);
    assert(str != NULL);

    // Define vertex buffer.
    const int len = strlen(str);
    const int glyph_size = 24*sizeof(float);
    float *buffer = malloc(len * glyph_size); // TODO Put a big buffer in font so we just allocate once.
    if (buffer == NULL) {
        DEBUG("malloc failed while allocating string '%s'.", str);
        return;
    }

    // Harfbuzz.

    hb_buffer_clear_contents(font->hb_buffer);
    hb_buffer_set_direction(font->hb_buffer, HB_DIRECTION_LTR);

    // Actually add the text and shape it.
    hb_buffer_add_utf8(font->hb_buffer, str, strlen(str), 0, strlen(str));
    hb_shape(font->hb_font, font->hb_buffer, NULL, 0);

    unsigned int glyph_count;
    //hb_glyph_info_t *glyph_info = hb_buffer_get_glyph_infos(font->hb_buffer, &glyph_count);
    hb_glyph_position_t *glyph_pos = hb_buffer_get_glyph_positions(font->hb_buffer, &glyph_count);

    const float scale_x = 2.0f / (64 * viewport_width);
    const float scale_y = 2.0f / (64 * viewport_height);

    // Adjust x based on the alignment.
    if (alignment != TEXT_ALIGN_LEFT) {

        float width = 0.0f;

        for (int i = 0; i < len; i++) {

            const char c = str[i];
            if (c < font->start || c > font->end) {
                DEBUG("Character '%c' is outside of glyph range '%c'-'%c'.", c, font->start, font->end);
                continue;
            }

            width += glyph_pos[i].x_advance * scale_x;
        }

        if      (alignment == TEXT_ALIGN_CENTER) x -= width/2;
        else if (alignment == TEXT_ALIGN_RIGHT)  x -= width;

    }

    // Fill the buffer.

    int buffer_i = 0;

    for (int i = 0; i < len; i++) {

        const char c = str[i];
        if (c < font->start || c > font->end) {
            DEBUG("Character '%c' is outside of glyph range '%c'-'%c'.", c, font->start, font->end);
            continue;
        }
        const GlyphInfo *gi = &font->glyph_info[c-font->start];

        if (gi->metrics.width > 0 && gi->metrics.height > 0) {

            float *buf = &buffer[buffer_i*24];

            const float x1 = x + (glyph_pos[i].x_offset + gi->metrics.horiBearingX) * scale_x;
            const float x2 = x1 + gi->metrics.width * scale_x;
            const float y1 = y - (gi->metrics.height - gi->metrics.horiBearingY - glyph_pos[i].y_offset)*scale_y;
            const float y2 = y1 + gi->metrics.height * scale_y;

            // Vertices --                  UV coordinates --
            buf[ 0] = x1;   buf[ 1] = y1;   buf[ 2] = gi->u1;    buf[ 3] = gi->v1;
            buf[ 4] = x1;   buf[ 5] = y2;   buf[ 6] = gi->u1;    buf[ 7] = gi->v2;
            buf[ 8] = x2;   buf[ 9] = y1;   buf[10] = gi->u2;    buf[11] = gi->v1;

            buf[12] = x1;   buf[13] = y2;   buf[14] = gi->u1;    buf[15] = gi->v2;
            buf[16] = x2;   buf[17] = y1;   buf[18] = gi->u2;    buf[19] = gi->v1;
            buf[20] = x2;   buf[21] = y2;   buf[22] = gi->u2;    buf[23] = gi->v2;

            buffer_i++;
        }

        x += glyph_pos[i].x_advance * scale_x;
        y -= glyph_pos[i].y_advance * scale_y;
    }

    text->buffer_len = buffer_i * glyph_size;

    // Generate vbo.
    glGenVertexArrays(1, &text->vao);
    glBindVertexArray(text->vao);
    glGenBuffers(1, &text->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, text->vbo);
    glBufferData(GL_ARRAY_BUFFER, text->buffer_len, buffer, GL_STREAM_DRAW);

    free(buffer);

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
}
