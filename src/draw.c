#include "draw.h"
#include "misc.h"
#include <assert.h>
#include <math.h>

static GLint gl_get_uniform(GLuint program, const char *id) {
    GLint out = glGetUniformLocation(program, id);
    if (out == -1) DEBUG("Couldn't find uniform '%s'.", id);
    return out;
}

static GLuint make_circle_texture(int size, float aa_threshold) {

    GLuint texture;
    glGenTextures(1, &texture);
    if (texture == 0) {
        DEBUG("glGenTextures failed.");
        return 0;
    }

    unsigned char *data = malloc(size*size);
    if (data == NULL) {
        DEBUG("malloc failed.");
        return 0;
    }

    const float r = size/2.0f;

    for (int x = 0; x < size; x++) {
        for (int y = 0; y < size; y++) {
            const float dx = x - r;
            const float dy = y - r;
            float d = sqrtf(dx*dx + dy*dy);
            d -= r;
            if (d < 0) {
                if (d > -aa_threshold) data[x + y*size] = 0xFF * -d/aa_threshold;
                else                   data[x + y*size] = 0xFF;
            } else {
                data[x + y*size] = 0x00;
            }
        }
    }

    glBindTexture(GL_TEXTURE_2D, texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, size, size, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    free(data);

    return texture;
}

void draw_context_init(DrawContext *dc, Catalog *catalog, float aspect, int width, int height, float hdpi, float vdpi) {

    assert(dc != NULL);

    dc->aspect = aspect;
    draw_resize(dc, width, height);
    dc->hdpi = hdpi;
    dc->vdpi = vdpi;

    dc->catalog = catalog;

    dc->bound_texture = 0;
    dc->bound_program = 0;

    // Text shader.
    shader_program_init(&dc->text_shader,
                        "../src/shaders/text.vs.glsl",
                        "../src/shaders/text.fs.glsl");
    catalog_add(dc->catalog, dc->text_shader.vert_path, shader_program_reload, &dc->text_shader);
    catalog_add(dc->catalog, dc->text_shader.frag_path, shader_program_reload, &dc->text_shader);

    // Quad shader.
    shader_program_init(&dc->quad_shader,
                        "../src/shaders/sprite.vs.glsl",
                        "../src/shaders/sprite.fs.glsl");
    catalog_add(dc->catalog, dc->quad_shader.vert_path, shader_program_reload, &dc->quad_shader);
    catalog_add(dc->catalog, dc->quad_shader.frag_path, shader_program_reload, &dc->quad_shader);

    // Load a circle mask texture.
    dc->circle_texture = make_circle_texture(512, 1.5);

    // Uniforms.
    dc->u_texture   = gl_get_uniform(dc->text_shader.gl_program, "texture");
    dc->u_color     = gl_get_uniform(dc->text_shader.gl_program, "color");
    dc->u_transform = gl_get_uniform(dc->text_shader.gl_program, "transform");

    glClearColor(0.2f, 0.2f, 0.2f, 1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

}

void draw_context_destroy(DrawContext *dc) {
    assert(dc != NULL);
    shader_program_destroy(&dc->text_shader);
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

void draw_set_color(DrawContext *dc, float r, float g, float b, float a) {
    assert(dc != NULL);
    GLuint gl_program = dc->text_shader.gl_program;
    if (dc->bound_program != gl_program) glUseProgram(gl_program);
    glUniform4f(dc->u_color, r, g, b, a);
}

void draw_string(DrawContext *dc, Font *font, const char *str, float x, float y, TextAlignment alignment) {

    assert(dc != NULL);
    assert(str != NULL);

    Text text;
    text_init(&text, font, str);

    draw_text(dc, &text, x, y, alignment);

    text_destroy(&text);
}

void draw_text(DrawContext *dc, Text *text, float x, float y, TextAlignment alignment) {

    assert(dc != NULL);
    assert(text != NULL);

    const float s = 2.0f / dc->width;

    if (alignment != TEXT_ALIGN_LEFT) {
        if      (alignment == TEXT_ALIGN_CENTER) x -= s*text->width/2;
        else if (alignment == TEXT_ALIGN_RIGHT)  x -= s*text->width;
    }

    const GLfloat transform[] = {
           s,         0.0f,    x,
        0.0f, s*dc->aspect,    y,
        0.0f,         0.0f, 1.0f
    };

    GLuint gl_texture = text->font->gl_texture;
    if (dc->bound_texture != gl_texture) glBindTexture(GL_TEXTURE_2D, gl_texture);

    GLuint gl_program = dc->text_shader.gl_program;
    if (dc->bound_program != gl_program) glUseProgram(gl_program);

    glBindVertexArray(text->vao);
    glUniform1i(dc->u_texture, 0);
    glUniformMatrix3fv(dc->u_transform, 1, GL_FALSE, transform);
    glDrawArrays(GL_TRIANGLES, 0, text->buffer_len/(4*sizeof(float)));
}

void draw_sprite(DrawContext *dc, Sprite *sprite, Texture *texture) {

    assert(dc != NULL);
    assert(sprite != NULL);
    assert(texture != NULL);

    const float s = 1.0f,
                x = 0.0f,
                y = 0.0f;

    const GLfloat transform[] = {
           s,         0.0f,    x,
        0.0f, s*dc->aspect,    y,
        0.0f,         0.0f, 1.0f
    };

    GLuint gl_texture = texture->gl_texture;
    if (dc->bound_texture != gl_texture) glBindTexture(GL_TEXTURE_2D, gl_texture);

    GLuint gl_program = dc->quad_shader.gl_program;
    if (dc->bound_program != gl_program) glUseProgram(gl_program);

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Sprite), sprite->buffer, GL_DYNAMIC_DRAW);

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

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4*sizeof(GLfloat),
        (void *) (2*sizeof(GLfloat))
    );
    glEnableVertexAttribArray(1);

    glUniformMatrix3fv(dc->u_transform, 1, GL_FALSE, transform);
    glDrawArrays(GL_TRIANGLES, 0, 6);

}

void draw_sprite_batch(DrawContext *dc, SpriteBatch *sprite_batch, Texture *texture, float x, float y) {

    assert(dc != NULL);
    assert(sprite_batch != NULL);
    assert(texture != NULL);

    const float s = 1.0f;

    const GLfloat transform[] = {
           s,         0.0f,    x,
        0.0f, s*dc->aspect,    y,
        0.0f,         0.0f, 1.0f
    };

    GLuint gl_texture = texture->gl_texture;
    if (dc->bound_texture != gl_texture) glBindTexture(GL_TEXTURE_2D, gl_texture);

    GLuint gl_program = dc->quad_shader.gl_program;
    if (dc->bound_program != gl_program) glUseProgram(gl_program);

    glBindVertexArray(sprite_batch->vao);

    glUniformMatrix3fv(dc->u_transform, 1, GL_FALSE, transform);
    glDrawArrays(GL_TRIANGLES, 0, 6*sprite_batch->sprites.count);

}

void draw_circle(DrawContext *dc, float x, float y, float r) {

    assert(dc != NULL);

    const float sx = 2.0f*r;
    const float sy = 2.0f*r*dc->aspect;

    const GLfloat transform[] = {
          sx, 0.0f, x,
        0.0f,   sy, y*dc->aspect,
        0.0f, 0.0f, 1.0f
    };

    GLuint gl_texture = dc->circle_texture;
    if (dc->bound_texture != gl_texture) glBindTexture(GL_TEXTURE_2D, gl_texture);

    GLuint gl_program = dc->text_shader.gl_program;
    if (dc->bound_program != gl_program) glUseProgram(gl_program);

    Sprite sprite;
    sprite_init(&sprite, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f);

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Sprite), sprite.buffer, GL_DYNAMIC_DRAW);

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

    glVertexAttribPointer(
        1,
        2,
        GL_FLOAT,
        GL_FALSE,
        4*sizeof(GLfloat),
        (void *) (2*sizeof(GLfloat))
    );
    glEnableVertexAttribArray(1);

    glUniformMatrix3fv(dc->u_transform, 1, GL_FALSE, transform);
    glDrawArrays(GL_TRIANGLES, 0, 6);


}
