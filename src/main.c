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
              //"/usr/share/fonts/truetype/liberation/LiberationSerif-Regular.ttf",
              //"/usr/share/fonts/truetype/dejavu/DejaVuSerif.ttf",
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
    text_init(&fps_text, &font, "-");

    Text static_text;
    char *str = load_entire_file("Makefile");
    text_init(&static_text, &font, str);

    SpriteBatch sprite_batch;
    sprite_batch_init(&sprite_batch, 1);
    int state = 0;

    while (1) {

        window_update(&window);
        if (window.input.quit) break;

        catalog_service(&catalog); // Make sure we hotload stuff.

        //const float speed = 2.0f;
        //y -= window.input.down  * speed*dc.aspect * window.dt;
        //y += window.input.up    * speed*dc.aspect * window.dt;
        //x -= window.input.left  * speed * window.dt;
        //x += window.input.right * speed * window.dt;

        // Adjust mouse position.
        int mouse_x = window.input.mouse_x + (dc.width  - window.width )/2;
        if      (mouse_x < 0)           mouse_x = 0;
        else if (mouse_x > dc.width-1)  mouse_x = dc.width-1;
        int mouse_y = window.input.mouse_y + (dc.height - window.height)/2;
        if      (mouse_y < 0)           mouse_y = 0;
        else if (mouse_y > dc.height-1) mouse_y = dc.height-1;

        x = (float) mouse_x / dc.width  * 2.0f - 1.0f;
        y = (float) mouse_y / dc.height * 2.0f - 1.0f;
        y = -y;

        average_add(&average, 1.0f / window.dt);

        char buffer[64];

        if (window.elapsed_ms - update_fps_every > last_fps_update) {

            last_fps_update = window.elapsed_ms;
            fps = average_calc(&average);
            
            // Update text.
            snprintf(buffer, 64, "[%5.1f]", fps);
            text_destroy(&fps_text);
            text_init(&fps_text, &font, buffer);

        }

        draw_clear(&dc);

        // Static text.
        draw_set_color(&dc, 0.2f, 0.2f, 0.2f, 1.0f);
        draw_text(&dc, &static_text, -0.95f + 0.01f, 0.8f + 0.01f, TEXT_ALIGN_LEFT);
        draw_set_color(&dc, 0.6f, 0.2f, 0.2f, 1.0f);
        draw_text(&dc, &static_text, -0.95f, 0.8f, TEXT_ALIGN_LEFT);

        // FPS average.
        draw_set_color(&dc, 0.2f, 0.2f, 0.2f, 1.0f);
        draw_text(&dc, &fps_text, -1.0f + 0.11f, -1.0f + 0.11f*dc.aspect, TEXT_ALIGN_LEFT);
        draw_set_color(&dc, 0.5f, 0.9f, 0.5f, 1.0f);
        draw_text(&dc, &fps_text, -1.0f + 0.1f, -1.0f + 0.1f*dc.aspect, TEXT_ALIGN_LEFT);

        // Moving text.
        snprintf(buffer, 64, "(%d, %d)", mouse_x, mouse_y);
        draw_set_color(&dc, 0.2f, 0.2f, 0.2f, 1.0f);
        draw_string(&dc, &font, buffer, x+0.01f, y+0.01f, TEXT_ALIGN_CENTER);
        draw_set_color(&dc, 0.1f, 0.8f, 0.9f, 1.0f);
        draw_string(&dc, &font, buffer, x, y, TEXT_ALIGN_CENTER);

        // Sprite of man.
        if (state != (window.elapsed_ms / 100) % 4) {
            state = (window.elapsed_ms / 100) % 4;

            Sprite sprite;
            sprite_init(&sprite, 0.0f, 0.0f, 0.3f, 0.5f, state*0.25f, 0.f, state*0.25f + 0.25f, 1.0f);
            sprite_batch_clear(&sprite_batch);
            sprite_batch_add(&sprite_batch, &sprite);
            sprite_batch_update(&sprite_batch);

        }
        draw_sprite_batch(&dc, &sprite_batch, &texture, x, y);

        window_redraw(&window);

    }

    sprite_batch_destroy(&sprite_batch);

    text_destroy(&static_text);
    text_destroy(&fps_text);

    draw_context_destroy(&dc);
    window_destroy(&window);

    catalog_destroy(&catalog);

    FT_Done_FreeType(ft);
    SDL_Quit();
    free(str);

    return 0;
}
