#ifndef SHADER_PROGRAM_H
#define SHADER_PROGRAM_H

#include <GL/glew.h>

struct ShaderProgram {
    char *vert_path,
         *frag_path;
    GLuint gl_program,
           gl_vert_shader,
           gl_frag_shader;
};
typedef struct ShaderProgram ShaderProgram;

void shader_program_init(ShaderProgram *shader_program, const char *vert_path, const char *frag_path);
void shader_program_reload(ShaderProgram *shader_program);
void shader_program_destroy(ShaderProgram *shader_program);

#endif
