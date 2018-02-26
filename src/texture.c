#include "texture.h"
#include "misc.h"
#include <stdlib.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void texture_init(Texture *t, const char *path) {

    t->path = copy_string(path);
    texture_reload(t);

    glGenTextures(1, &t->gl_texture);
    if (t->gl_texture == 0) {
        DEBUG("glGenTextures failed.");
        return;
    }
}

void texture_destroy(Texture *t) {
    free(t->path);
    glDeleteTextures(1, &t->gl_texture);
}

void texture_reload(Texture *t) {

    int w, h, n;
    unsigned char *data = stbi_load(t->path, &w, &h, &n, 0);

    if (data == NULL) {
        DEBUG("Couldn't load image '%s'.", t->path);
        return;
    }

    glBindTexture(GL_TEXTURE_2D, t->gl_texture);

    if (n == 1) {

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, w, h, 0, GL_RED, GL_UNSIGNED_BYTE, data);

    } else if (n == 3) {

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);

    } else if (n == 4) {

        glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    } else {
        DEBUG("n = %d not handled.", n);
    }

    stbi_image_free(data);
}
