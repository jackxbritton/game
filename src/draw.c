#include "draw.h"
#include "misc.h"
#include <assert.h>

static GLint gl_get_uniform(GLuint program, const char *id) {
    GLint out = glGetUniformLocation(program, id);
    if (out == -1) DEBUG("Couldn't find uniform '%s'.", id);
    return out;
}

void draw_context_init(DrawContext *dc, float aspect, float hdpi, float vdpi) {

    assert(dc != NULL);

    dc->aspect = aspect;

    catalog_init(&dc->catalog);

    // Text shader.
    shader_program_init(&dc->text_shader,
                        "../src/shaders/mask.vs.glsl",
                        "../src/shaders/mask.fs.glsl");
    catalog_add(&dc->catalog, dc->text_shader.vert_path, shader_program_reload, &dc->text_shader);
    catalog_add(&dc->catalog, dc->text_shader.frag_path, shader_program_reload, &dc->text_shader);

    // Uniforms.
    dc->u_texture   = gl_get_uniform(dc->text_shader.gl_program, "texture");
    dc->u_color     = gl_get_uniform(dc->text_shader.gl_program, "color");
    dc->u_transform = gl_get_uniform(dc->text_shader.gl_program, "transform");

    // FreeType and our font.
    if (FT_Init_FreeType(&dc->ft)) DEBUG("FT_Init_FreeType failed.");
    font_init(&dc->font,
              &dc->ft,
              "/usr/share/fonts/truetype/liberation/LiberationSerif-Regular.ttf",
              48, (int) hdpi, (int) vdpi);

    // Set texture units.
    glBindTextureUnit(0, dc->font.gl_texture);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

}

void draw_context_destroy(DrawContext *dc) {
    assert(dc != NULL);
    catalog_destroy(&dc->catalog);
    shader_program_destroy(&dc->text_shader);
    font_destroy(&dc->font);
    FT_Done_FreeType(dc->ft);
}

void draw_resize(DrawContext *dc, int w, int h) {

    assert(dc != NULL);

    const float ratio = (float) w / h;

    if (ratio > dc->aspect) {
        dc->width  = h*dc->aspect;
        dc->height = h;
        glViewport((w - dc->width)/2, 0, dc->width, dc->height);
    } else {
        dc->width  = w;
        dc->height = w/dc->aspect;
        glViewport(0, (h - dc->height)/2, dc->width, dc->height);
    }
}

void draw_clear(DrawContext *dc) {
    glClear(GL_COLOR_BUFFER_BIT);
}

void draw_string(DrawContext *dc, const char *str, float x, float y, TextAlignment alignment) {

    assert(dc != NULL);
    assert(str != NULL);

    Text text;
    text_init(&text, &dc->font, str, dc->text_shader.gl_program);

    draw_text(dc, &text, x, y, alignment);

    text_destroy(&text);
}

void draw_text(DrawContext *dc, Text *text, float x, float y, TextAlignment alignment) {

    assert(dc != NULL);
    assert(text != NULL);

    const float s = 2.0f / dc->width;

    if (alignment != TEXT_ALIGN_LEFT) {
        if      (alignment == TEXT_ALIGN_CENTER) x -= text->width*s/2;
        else if (alignment == TEXT_ALIGN_RIGHT)  x -= text->width*s;
    }

    const GLfloat transform[] = {
           s,         0.0f,    x,
        0.0f, s*dc->aspect,    y,
        0.0f,         0.0f, 1.0f
    };

    glUseProgram(dc->text_shader.gl_program);
    glBindVertexArray(text->vao);
    //glUniform4f(dc->u_color, 0.1f, 0.8f, 0.9f, 1.0f);
    glUniform1i(dc->u_texture, 0);
    glUniformMatrix3fv(dc->u_transform, 1, GL_FALSE, transform);
    glDrawArrays(GL_TRIANGLES, 0, text->buffer_len/(4*sizeof(float)));
}
