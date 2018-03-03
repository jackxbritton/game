#include "misc.h"
#include "window.h"
#include "draw.h"
#include "average.h" // For FPS averaging..

#include "physics.h"

struct Spaceship {
    float angle,
          position_x,
          position_y,
          velocity_x,
          velocity_y;
};
typedef struct Spaceship Spaceship;

void draw_spaceship(DrawContext *dc, Texture *texture, int spaceship, int flap, float x, float y, float angle);

/*


-- THOUGHTS --
Collision and stuff!
Colliders will all be in an array or memory pool.
Functions will operate on this array. Details:

    while dt > max:
        dt -= max
        update(max)
    update(dt)

    eventually broadphase (quadtrees?)

*/

int main(int argc, char *argv[]) {

    // TODO Test physics.c.

    RigidBody rb;
    rigid_body_init(&rb, 0);
    rb.collider_type = COLLIDER_CIRCLE;
    rb.collider.circle.radius = 0.4f;

    Vector2 v;
    v.x =  4.0f;
    v.y = -2.0f;
    Vector2 vn = v;
    vector2_normalize(&vn);
    DEBUG("l=%f, l^2=%f, vn=(%f, %f)",
          vector2_length(&v),
          vector2_length_squared(&v),
          vn.x, vn.y);

    return 0;

    
    // ^ Delete that return statement!


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

    Texture dude_texture;
    texture_init(&dude_texture, "../assets/foo.png");
    catalog_add(&catalog, dude_texture.path, texture_reload, &dude_texture);

    Texture galaga_texture;
    texture_init(&galaga_texture, "../assets/galaga.png");
    catalog_add(&catalog, galaga_texture.path, texture_reload, &galaga_texture);

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
    int state;

    struct Spaceship player;
    player.angle = M_PI/2.0f;
    player.position_x = 0.0f;
    player.position_y = -0.45f;

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
        draw_set_color(&dc, 0.1f, 0.1f, 0.1f, 1.0f);
        draw_text(&dc, &static_text, -0.95f + 0.01f, 0.8f + 0.01f, TEXT_ALIGN_LEFT);
        draw_set_color(&dc, 0.6f, 0.2f, 0.2f, 1.0f);
        draw_text(&dc, &static_text, -0.95f, 0.8f, TEXT_ALIGN_LEFT);

        // FPS average.
        draw_set_color(&dc, 0.1f, 0.1f, 0.1f, 1.0f);
        draw_text(&dc, &fps_text, -1.0f + 0.11f, -1.0f + 0.11f*dc.aspect, TEXT_ALIGN_LEFT);
        draw_set_color(&dc, 0.5f, 0.9f, 0.5f, 1.0f);
        draw_text(&dc, &fps_text, -1.0f + 0.1f, -1.0f + 0.1f*dc.aspect, TEXT_ALIGN_LEFT);

        // Moving text.
        snprintf(buffer, 64, "(%d, %d)", mouse_x, mouse_y);
        draw_set_color(&dc, 0.1f, 0.1f, 0.1f, 1.0f);
        draw_string(&dc, &font, buffer, x+0.01f, y+0.01f, TEXT_ALIGN_CENTER);
        draw_set_color(&dc, 0.1f, 0.8f, 0.9f, 1.0f);
        draw_string(&dc, &font, buffer, x, y, TEXT_ALIGN_CENTER);

        // Sprite of man.
        if (state != (window.elapsed_ms / 100) % 4) {
            state = (window.elapsed_ms / 100) % 4;

            Sprite sprite;
            sprite_init(&sprite, 0.0f, 0.0f, 0.3f, 0.5f, state*0.25f, 0.0f, state*0.25f + 0.25f, 1.0f);
            sprite_batch_clear(&sprite_batch);
            sprite_batch_add(&sprite_batch, &sprite);
            sprite_batch_update(&sprite_batch);

        }

        draw_sprite_batch(&dc, &sprite_batch, &dude_texture, x, y);

        // Sprite of spaceship.

        const float speed = 1.0f;
        player.position_x += (window.input.right - window.input.left) * speed*window.dt;

        snprintf(buffer, 64, "%f", player.angle);
        draw_set_color(&dc, 1.0f, 1.0f, 1.0f, 1.0f);
        draw_string(&dc, &font, buffer, -0.2f, 0.0f, TEXT_ALIGN_CENTER);

        float angle = (float) window.elapsed_ms / 100;
        while (angle < 0.0f)       angle += 2.0f*M_PI;
        while (angle >= 2.0f*M_PI) angle -= 2.0f*M_PI;
        int flap = (window.elapsed_ms / 200) % 2;
        draw_spaceship(&dc, &galaga_texture, 2, flap, player.position_x, player.position_y, player.angle);

        window_redraw(&window);

    }

    sprite_batch_destroy(&sprite_batch);
    texture_destroy(&dude_texture);
    texture_destroy(&galaga_texture);

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

void draw_spaceship(DrawContext *dc, Texture *texture, int spaceship, int flap, float x, float y, float angle) {

    int state = 24 - (int) (angle * 24/(2.0f*M_PI));
    state = (state + 12) % 24;

    const int w = texture->width,
              h = texture->height;
    const int r = state/6;
    state = state % 6;

    const float s = 0.15f;
    x -= s/2;
    y -= s/2;

    // Draw a spaceship.

    float u1, v1,
          u2, v2;

    if (state == 0 && flap) {
        u1 = (13.0f + 24.0f*7) / w;
        v1 = (309.0f - 24.0f*spaceship) / h;
        u2 = u1 + 24.0f/w;
        v2 = v1 + 24.0f/h;
    } else if (r == 0) {
        u1 = (13.0f + 24.0f*state) / w;
        v1 = (309.0f - 24.0f*spaceship) / h;
        u2 = u1 + 24.0f/w;
        v2 = v1 + 24.0f/h;
    } else if (r == 1) {
        u1 = (13.0f + 24.0f*(6-state) - 3.0f + 24.0f) / w;
        v1 = (309.0f - 24.0f*spaceship) / h;
        u2 = u1 - 24.0f/w;
        v2 = v1 + 24.0f/h;
    } else if (r == 2) {
        u1 = (13.0f + 24.0f*state - 3.0f + 24.0f) / w;
        v1 = (309.0f - 24.0f*spaceship - 1.0f + 24.0f) / h;
        u2 = u1 - 24.0f/w;
        v2 = v1 - 24.0f/h;
    } else {
        u1 = (13.0f + 24.0f*(6-state)) / w;
        v1 = (309.0f - 24.0f*spaceship - 1.0f + 24.0f) / h;
        u2 = u1 + 24.0f/w;
        v2 = v1 - 24.0f/h;
    }

    Sprite sprite;
    sprite_init(&sprite, x, y, s, s,
                         u1, v1,
                         u2, v2);

    draw_sprite(dc, &sprite, texture);

}
