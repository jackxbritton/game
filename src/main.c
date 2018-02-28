#include "misc.h"
#include "window.h"
#include "draw.h"

int main(int argc, char *argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        DEBUG("%s", SDL_GetError());
        return 1;
    }

    Window window;
    window_init(&window);

    DrawContext dc;
    draw_context_init(&dc, 16.0f/9.0f, window.hdpi, window.vdpi);

    // This stuff is sort of hacky,
    // but I think I reduced the evil
    // as much as possible.
    window.draw_context = &dc;
    draw_resize(&dc, window.width, window.height);

    while (1) {

        window_update(&window);
        if (window.input.quit) break;

        catalog_service(&dc.catalog); // Make sure we hotload stuff.

        draw_clear(&dc);

        if (window.input.up)         draw_string(&dc, "up");
        if (window.input.down)       draw_string(&dc, "down");
        if (window.input.left)       draw_string(&dc, "left");
        if (window.input.right)      draw_string(&dc, "right");
        if (window.input.mouse_left) draw_string(&dc, "mouse");

        char buffer[64];
        snprintf(buffer, 64, "%dx%d, %dx%d", window.width, window.height, dc.width, dc.height);
        draw_string(&dc, buffer);

        window_redraw(&window);

    }

    draw_context_destroy(&dc);
    window_destroy(&window);

    SDL_Quit();

    return 0;
}
