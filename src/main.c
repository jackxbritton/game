#include "misc.h"
#include "window.h"
#include "draw.h"

int main(int argc, char *argv[]) {

    Window window;
    window_init(&window);

    DrawContext dc;
    draw_context_init(&dc, 16.0f/9.0f, window.hdpi);

    // This stuff is sort of hacky,
    // but I think I reduced the evil
    // as much as possible.
    window.draw_context = &dc;
    draw_resize(&dc, window.width, window.height);

    while (1) {

        window_update(&window);
        if (window.input.quit) break;

        catalog_service(&dc.catalog); // Make sure we hotload stuff.

        glClear(GL_COLOR_BUFFER_BIT);

        draw_string(&dc, "yoo hoo");

        window_redraw(&window);

    }

    draw_context_destroy(&dc);
    window_destroy(&window);

    return 0;
}
