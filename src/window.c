#include "window.h"
#include <GL/glew.h>
#include "misc.h"
#include <string.h>

void window_init(Window *w) {

    w->width  = 640;
    w->height = 480;

    // SDL window.
    w->window = SDL_CreateWindow(
        "game",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        w->width, w->height,
        SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE
    );
    if (w->window == NULL) {
        DEBUG("%s", SDL_GetError());
        return;
    }

    // Get the DPI.
    int display = SDL_GetWindowDisplayIndex(w->window);
    if (display < 0) {
        DEBUG("%s", SDL_GetError());
        return;
    }
    if (SDL_GetDisplayDPI(display, &w->ddpi, &w->hdpi, &w->vdpi) < 0) {
        DEBUG("%s", SDL_GetError());
        return;
    }
    
    SDL_ShowCursor(SDL_DISABLE);

    // OpenGL context.
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    w->context = SDL_GL_CreateContext(w->window);
    if (w->context == NULL) {
        DEBUG("%s", SDL_GetError());
        return;
    }

    SDL_GL_SetSwapInterval(GL_TRUE); // vsync.

    // GLEW.
    glewExperimental = GL_TRUE;
    glewInit();

    // Initialize things.
    memset(&w->input, 0, sizeof(w->input));
    w->draw_context = NULL;

    w->elapsed_ms = 0;
}

void window_destroy(Window *w) {
    SDL_GL_DeleteContext(w->context);
    SDL_DestroyWindow(w->window);
}

void window_update(Window *w) {

    SDL_Event event;
    while (SDL_PollEvent(&event)) {

        if (event.type == SDL_QUIT) w->input.quit = 1;

        if (event.type == SDL_WINDOWEVENT) {
            if (event.window.event == SDL_WINDOWEVENT_RESIZED) {
                w->width  = event.window.data1;
                w->height = event.window.data2;
                if (w->draw_context != NULL) {
                    draw_resize(w->draw_context, w->width, w->height);
                }
                continue;
            }
        }

        if (event.type == SDL_KEYDOWN || event.type == SDL_KEYUP) {
            int is_pressed = event.type == SDL_KEYDOWN;
            if      (event.key.keysym.sym == SDLK_w) w->input.up    = is_pressed;
            else if (event.key.keysym.sym == SDLK_s) w->input.down  = is_pressed;
            else if (event.key.keysym.sym == SDLK_a) w->input.left  = is_pressed;
            else if (event.key.keysym.sym == SDLK_d) w->input.right = is_pressed;
            continue;
        }

        if (event.type == SDL_MOUSEMOTION) {
            w->input.mouse_x = event.motion.x;
            w->input.mouse_y = event.motion.y;
            continue;
        }

        if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEBUTTONUP) {
            int is_pressed = event.type == SDL_MOUSEBUTTONDOWN;
            if      (event.button.button == SDL_BUTTON_LEFT)  w->input.mouse_left  = is_pressed;
            else if (event.button.button == SDL_BUTTON_RIGHT) w->input.mouse_right = is_pressed;
            continue;
        }
    }

    Uint32 ms_now = SDL_GetTicks();
    w->dt = (ms_now - w->elapsed_ms) / 1000.0f;
    w->elapsed_ms = ms_now;

}

void window_redraw(Window *w) {
    SDL_GL_SwapWindow(w->window);
}
