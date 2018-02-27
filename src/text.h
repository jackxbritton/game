#ifndef TEXT_H
#define TEXT_H

#include "font.h"
#include <GL/glew.h>

struct Text {
    Font *font;
    char *str;
    float width, height;
    GLuint vao, vbo;
    size_t buffer_len;
};
typedef struct Text Text;

void text_init(Text *text, Font *font, const char *str, GLuint program);
void text_destroy(Text *text);

#endif
