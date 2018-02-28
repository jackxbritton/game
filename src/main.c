#include "misc.h"
#include "window.h"
#include "draw.h"
#include "average.h" // For FPS averaging..

int main(int argc, char *argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        DEBUG("%s", SDL_GetError());
        return 1;
    }

    Window window;
    window_init(&window);

    DrawContext dc;
    const float aspect = 16.0f/9.0f;
    draw_context_init(&dc, aspect, window.hdpi, window.vdpi);

    // This stuff is sort of hacky,
    // but I think I reduced the evil
    // as much as possible.
    window.draw_context = &dc;
    draw_resize(&dc, window.width, window.height);

    float x = 0.0f,
          y = 0.0f;

    Average average;
    average_init(&average, 64);

    while (1) {

        window_update(&window);
        if (window.input.quit) break;

        catalog_service(&dc.catalog); // Make sure we hotload stuff.

        const float speed = 0.5f;
        y -= window.input.down  * speed * window.dt;
        y += window.input.up    * speed * window.dt;
        x -= window.input.left  * speed * window.dt;
        x += window.input.right * speed * window.dt;

        average_add(&average, 1.0f / window.dt);

        draw_clear(&dc);

        // Moving text.
        char buffer[64];
        snprintf(buffer, 64, "(%1.2f, %1.2f)", x, y);
        draw_string(&dc, buffer, x, y, TEXT_ALIGN_CENTER);

        // FPS average.
        snprintf(buffer, 64, "[%3.1f]", average_calc(&average));
        draw_string(&dc, buffer, -1.0f + 0.1f, -1.0f + 0.1f*aspect, TEXT_ALIGN_LEFT);

        window_redraw(&window);

    }

    draw_context_destroy(&dc);
    window_destroy(&window);

    SDL_Quit();

    return 0;
}
