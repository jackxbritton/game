#ifndef TEXTURE_H
#define TEXTURE_H

#include <GL/glew.h>

struct Texture {
    char *path;
    GLuint gl_texture;
};
typedef struct Texture Texture;

void texture_init(Texture *texture, const char *path);
void texture_destroy(Texture *texture);
void texture_reload(Texture *texture);

#endif
