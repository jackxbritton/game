#include "misc.h"
#include "window.h"
#include "shader_program.h"
#include "texture.h"
#include "font.h"
#include "text.h"
#include "catalog.h"

void init_buffer(GLuint program);

int main(int argc, char *argv[]) {

    // Window.
    Window window;
    window_init(&window);

    // Catalog for asset hotloading.
    Catalog catalog;
    catalog_init(&catalog);

    // Basic shader program.
    ShaderProgram sp;
    shader_program_init(&sp, "../src/shaders/mask.vs.glsl", "../src/shaders/mask.fs.glsl");
    catalog_add(&catalog, sp.vert_path, shader_program_reload, &sp);
    catalog_add(&catalog, sp.frag_path, shader_program_reload, &sp);

    init_buffer(sp.gl_program);

    // Uniform.
    GLint u_texture = glGetUniformLocation(sp.gl_program, "texture");
    if (u_texture == -1) DEBUG("Couldn't find u_texture.");
    GLint u_color = glGetUniformLocation(sp.gl_program, "color");
    if (u_color == -1) DEBUG("Couldn't find u_color.");

    // Texture.
    //Texture texture;
    //texture_init(&texture, "../assets/test.png");
    //catalog_add(&catalog, texture.path, texture_reload, &texture);
    //glBindTextureUnit(0, texture.gl_texture);

    // Font.
    FT_Library ft;
    if (FT_Init_FreeType(&ft)) DEBUG("FT_Init_FreeType failed.");

    Font font;
    font_init(&font, &ft, "/usr/share/fonts/truetype/liberation/LiberationSerif-Regular.ttf");
    //font_init(&font, &ft, "/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf");
    glBindTextureUnit(0, font.gl_texture);

    // Example text.
    Text text;
    text_init(&text, &font, "T. AV. y g oh", sp.gl_program);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);

    while (window_update(&window)) {

        catalog_service(&catalog);

        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(sp.gl_program);
        glUniform1i(u_texture, 0);
        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        glUseProgram(sp.gl_program);
        glBindVertexArray(text.vao);
        glUniform4f(u_color, 0.1f, 0.8f, 0.9f, 1.0f);
        glUniform1i(u_texture, 0);
        glDrawArrays(GL_TRIANGLES, 0, text.buffer_len/(4*sizeof(float)));

        window_redraw(&window);

    }

    text_destroy(&text);
    catalog_destroy(&catalog);
    font_destroy(&font);
    //texture_destroy(&texture);
    shader_program_destroy(&sp);
    window_destroy(&window);

    return 0;
}

void init_buffer(GLuint program) {

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);

    const GLfloat quad[] = {
        -0.5f,-0.5f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.0f, 1.0f,
         0.5f,-0.5f, 1.0f, 0.0f,
         0.5f, 0.5f, 1.0f, 1.0f
    };

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);

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
