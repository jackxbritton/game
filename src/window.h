#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>
#include "draw.h"

struct Window {

    SDL_Window *window;
    SDL_GLContext context;
    
    DrawContext *draw_context; // For resizing.

    int width,
        height;

    float ddpi,
          hdpi,
          vdpi;

    struct {
        int quit,
            up,
            down,
            left,
            right,
            mouse_x,
            mouse_y,
            mouse_left,
            mouse_right;
    } input;

    Uint32 ms_last;
    float dt; // In seconds.

};
typedef struct Window Window;

void window_init(Window *w);
void window_destroy(Window *w);

void window_update(Window *w);
void window_redraw(Window *w);

#endif
