#ifndef TEXT_H
#define TEXT_H

#include "font.h"
#include <GL/glew.h>

struct Text {
    GLuint vao, vbo;
    size_t buffer_len;
};
typedef struct Text Text;

enum TextAlignment {
    TEXT_ALIGN_LEFT,
    TEXT_ALIGN_CENTER,
    TEXT_ALIGN_RIGHT
};
typedef enum TextAlignment TextAlignment;

void text_init(Text *text, Font *font,
               const char *str, TextAlignment alignment,
               GLuint program, int viewport_width, int viewport_height);
void text_destroy(Text *text);

#endif
