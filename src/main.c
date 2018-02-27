#include "misc.h"
#include "window.h"
#include "draw.h"

int main(int argc, char *argv[]) {

    Window window;
    window_init(&window);

    DrawContext dc;
    draw_context_init(&dc);

    while (window_update(&window)) {

        catalog_service(&dc.catalog); // Hotload junk.

        glClear(GL_COLOR_BUFFER_BIT);

        draw_string(&dc, "Hello!!");

        window_redraw(&window);

    }

    draw_context_destroy(&dc);
    window_destroy(&window);

    return 0;
}
