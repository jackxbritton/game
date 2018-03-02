#include "misc.h"
#include "window.h"
#include "draw.h"
#include "average.h" // For FPS averaging..

int main(int argc, char *argv[]) {

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        DEBUG("%s", SDL_GetError());
        return 1;
    }

    FT_Library ft;
    if (FT_Init_FreeType(&ft)) {
        DEBUG("FT_Init_FreeType failed.");
        SDL_Quit();
        return 1;
    }

    Window window;
    window_init(&window);

    Catalog catalog;
    catalog_init(&catalog);

    DrawContext dc;
    draw_context_init(&dc, &catalog, 16.0f/9.0f, window.width, window.height, window.hdpi, window.vdpi);

    // This stuff is sort of hacky,
    // but I think I reduced the evil
    // as much as possible.
    window.draw_context = &dc;

    Font font;
    font_init(&font, &ft,
              "/usr/share/fonts/truetype/freefont/FreeSerif.ttf",
              72, dc.hdpi, dc.vdpi);

    Texture texture;
    texture_init(&texture, "../assets/foo.png");
    catalog_add(&catalog, texture.path, texture_reload, &texture);

    float x = 0.0f,
          y = 0.0f;

    // FPS counter stuff.
    const int update_fps_every = 50;
    int last_fps_update = 0;
    float fps = 0.0f;

    Average average;
    average_init(&average, 32);

    Text fps_text;
    text_init(&fps_text, &font, "-", dc.text_shader.gl_program);

    Text static_text;
    char *str = load_entire_file("Makefile");
    text_init(&static_text, &font, str, dc.text_shader.gl_program);
    free(str);

    while (1) {

        window_update(&window);
        if (window.input.quit) break;

        catalog_service(&catalog); // Make sure we hotload stuff.

        //const float speed = 2.0f;
        //y -= window.input.down  * speed*dc.aspect * window.dt;
        //y += window.input.up    * speed*dc.aspect * window.dt;
        //x -= window.input.left  * speed * window.dt;
        //x += window.input.right * speed * window.dt;
        x = (float) window.input.mouse_x / dc.width  * 2.0f - 1.0f;
        y = (float) window.input.mouse_y / dc.height * 2.0f - 1.0f;
        y = -y;

        average_add(&average, 1.0f / window.dt);

        char buffer[64];

        if (window.elapsed_ms - update_fps_every > last_fps_update) {

            last_fps_update = window.elapsed_ms;
            fps = average_calc(&average);
            
            // Update text.
            snprintf(buffer, 64, "[%5.1f]", fps);
            text_destroy(&fps_text);
            text_init(&fps_text, &font, buffer, dc.text_shader.gl_program);

        }

        draw_clear(&dc);

        // Static text.
        glUniform4f(dc.u_color, 0.8f, 0.3f, 0.3f, 0.4f);
        draw_text(&dc, &static_text, -1.0f + 0.1f, 1.0f, TEXT_ALIGN_LEFT);

        // FPS average.
        glUniform4f(dc.u_color, 0.2f, 0.2f, 0.2f, 1.0f);
        draw_text(&dc, &fps_text, -1.0f + 0.11f, -1.0f + 0.11f*dc.aspect, TEXT_ALIGN_LEFT);
        glUniform4f(dc.u_color, 0.5f, 0.9f, 0.5f, 1.0f);
        draw_text(&dc, &fps_text, -1.0f + 0.1f, -1.0f + 0.1f*dc.aspect, TEXT_ALIGN_LEFT);

        // Moving text.
        snprintf(buffer, 64, "(%d, %d)", window.input.mouse_x, window.input.mouse_y);
        glUniform4f(dc.u_color, 0.2f, 0.2f, 0.2f, 1.0f);
        draw_string(&dc, &font, buffer, x+0.01f, y+0.01f, TEXT_ALIGN_CENTER);
        glUniform4f(dc.u_color, 0.1f, 0.8f, 0.9f, 1.0f);
        draw_string(&dc, &font, buffer, x, y, TEXT_ALIGN_CENTER);

        // Sprite of man.
        int state = (window.elapsed_ms / 100) % 4;
        Sprite sprite;
        sprite_init(&sprite, x, y, x+0.3f, y+0.5f, state*0.25f, 0.f, state*0.25f + 0.25f, 1.0f);
        draw_sprite(&dc, &sprite, &texture);

        glUseProgram(dc.text_shader.gl_program);

        window_redraw(&window);

    }

    text_destroy(&static_text);
    text_destroy(&fps_text);

    draw_context_destroy(&dc);
    window_destroy(&window);

    catalog_destroy(&catalog);

    FT_Done_FreeType(ft);
    SDL_Quit();

    return 0;
}
