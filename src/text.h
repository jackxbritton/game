#ifndef TEXT_H
#define TEXT_H

#include "font.h"
#include <GL/glew.h>

struct Text {
    Font *font;
    GLuint vao, vbo;
    size_t buffer_len;
    float width,
          height;
};
typedef struct Text Text;

void text_init(Text *text, Font *font, const char *str);
void text_destroy(Text *text);

#endif
