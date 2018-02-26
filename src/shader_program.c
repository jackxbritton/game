#include "shader_program.h"
#include "misc.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

GLuint gl_create_shader(GLenum type, const char *src);

void shader_program_init(ShaderProgram *sp, const char *vert_path, const char *frag_path) {
    assert(sp != NULL);
    sp->vert_path = copy_string(vert_path);
    sp->frag_path = copy_string(frag_path);

    // Create the program.
    sp->gl_program = glCreateProgram();
    if (sp->gl_program == 0) {
        DEBUG("glCreateProgram failed for vert_path='%s', frag_path='%s'.", sp->vert_path, sp->frag_path);
        return;
    }

    shader_program_reload(sp);
    return;
}

void shader_program_reload(ShaderProgram *sp) {

    assert(sp != NULL);

    char *vert_src = load_entire_file(sp->vert_path);
    if (vert_src == NULL) return;
    char *frag_src = load_entire_file(sp->frag_path);
    if (frag_src == NULL) {
        free(vert_src);
        return;
    }

    sp->gl_vert_shader = gl_create_shader(GL_VERTEX_SHADER, vert_src);
    if (sp->gl_vert_shader == 0) return;
    sp->gl_frag_shader = gl_create_shader(GL_FRAGMENT_SHADER, frag_src);
    if (sp->gl_frag_shader == 0) {
        glDeleteShader(sp->gl_vert_shader);
        return;
    }

    // Attach and link.
    glAttachShader(sp->gl_program, sp->gl_vert_shader);
    glAttachShader(sp->gl_program, sp->gl_frag_shader);
    glLinkProgram(sp->gl_program);

    // Detach and delete.
    glDetachShader(sp->gl_program, sp->gl_vert_shader);
    glDeleteShader(sp->gl_vert_shader);
    glDetachShader(sp->gl_program, sp->gl_frag_shader);
    glDeleteShader(sp->gl_frag_shader);

    // Assess errors.
    GLint status;
    glGetProgramiv(sp->gl_program, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        char error[1024];
        glGetProgramInfoLog(sp->gl_program, 1024, NULL, error);
        DEBUG("glLinkProgram failed: '%s'.", error);
    }

    free(vert_src);
    free(frag_src);
}

GLuint gl_create_shader(GLenum type, const char *src) {

    GLuint shader = glCreateShader(type);
    if (shader == 0) {
        DEBUG("glCreateShader failed.");
        return 0;
    }

    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_FALSE) {
        char error[1024];
        glGetShaderInfoLog(shader, 1024, NULL, error);
        DEBUG("glCompileShader failed: '%s'.", error);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

void shader_program_destroy(ShaderProgram *sp) {
    assert(sp != NULL);
    free(sp->vert_path);
    free(sp->frag_path);
    glDeleteProgram(sp->gl_program);
}
