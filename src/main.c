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
    draw_context_init(&dc, 16.0f/9.0f, window.hdpi, window.vdpi);

    // This stuff is sort of hacky,
    // but I think I reduced the evil
    // as much as possible.
    window.draw_context = &dc;
    draw_resize(&dc, window.width, window.height);

    float x = 0.0f,
          y = 0.0f;

    // FPS counter stuff.
    const int update_fps_every = 50;
    int last_fps_update = 0;
    float fps = 0.0f;

    Average average;
    average_init(&average, 64);

    Text fps_text;
    text_init(&fps_text, &dc.font, "-", dc.text_shader.gl_program);

    Text static_text;
    char *str = load_entire_file("Makefile");
    text_init(&static_text, &dc.font, str, dc.text_shader.gl_program);
    free(str);

    while (1) {

        window_update(&window);
        if (window.input.quit) break;

        catalog_service(&dc.catalog); // Make sure we hotload stuff.

        const float speed = 2.0f;
        y -= window.input.down  * speed*dc.aspect * window.dt;
        y += window.input.up    * speed*dc.aspect * window.dt;
        x -= window.input.left  * speed * window.dt;
        x += window.input.right * speed * window.dt;

        average_add(&average, 1.0f / window.dt);

        char buffer[64];

        if (window.elapsed_ms - update_fps_every > last_fps_update) {

            last_fps_update = window.elapsed_ms;
            fps = average_calc(&average);
            
            // Update text.
            snprintf(buffer, 64, "[%3.1f]", fps);
            text_destroy(&fps_text);
            text_init(&fps_text, &dc.font, buffer, dc.text_shader.gl_program);

        }

        draw_clear(&dc);

        // Static text.
        glUniform4f(dc.u_color, 0.8f, 0.3f, 0.3f, 0.4f);
        draw_text(&dc, &static_text, -1.0f + 0.1f, 1.0f, TEXT_ALIGN_RIGHT);

        // FPS average.
        glUniform4f(dc.u_color, 0.5f, 0.9f, 0.5f, 1.0f);
        draw_text(&dc, &fps_text, -1.0f + 0.1f, -1.0f + 0.1f*dc.aspect, TEXT_ALIGN_LEFT);

        // Moving text.
        snprintf(buffer, 64, "(%d, %d)", window.input.mouse_x, window.input.mouse_y);
        glUniform4f(dc.u_color, 0.1f, 0.8f, 0.9f, 1.0f);
        draw_string(&dc, buffer, x, y, TEXT_ALIGN_CENTER);

        window_redraw(&window);

    }

    text_destroy(&static_text);
    text_destroy(&fps_text);

    draw_context_destroy(&dc);
    window_destroy(&window);

    SDL_Quit();

    return 0;
}
