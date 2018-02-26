#ifndef WINDOW_H
#define WINDOW_H

#include <SDL2/SDL.h>

struct Window {

    int width,
        height;
    SDL_Window *window;
    SDL_GLContext context;

    struct {
        int up,
            down,
            left,
            right,
            mouse_x,
            mouse_y,
            mouse_left,
            mouse_right;
    } input;

};
typedef struct Window Window;

void window_init(Window *w);
void window_destroy(Window *w);

int window_update(Window *w);
void window_redraw(Window *w);

#endif
