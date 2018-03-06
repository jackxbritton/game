#include "misc.h"
#include "window.h"
#include "draw.h"
#include "average.h" // For FPS averaging..
#include "physics.h"

void collision_callback(void *va, void *vb) {
    int a = (int) ((long) va);
    int b = (int) ((long) vb);
    DEBUG("%d %d", a, b);
}

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

    Texture dude_texture;
    texture_init(&dude_texture, "../assets/foo.png");
    catalog_add(&catalog, dude_texture.path, texture_reload, &dude_texture);

    Texture galaga_texture;
    texture_init(&galaga_texture, "../assets/galaga.png");
    catalog_add(&catalog, galaga_texture.path, texture_reload, &galaga_texture);

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

    int state;

    // Test rigid bodies.

    PhysicsScene physics_scene;
    physics_scene_init(&physics_scene);

    RigidBody *cursor = physics_scene_add(&physics_scene);

    cursor->position.x = 0.4f;
    cursor->position.y = 0.4f;
    cursor->collider_type = COLLIDER_CIRCLE;
    cursor->collider.circle.radius = 0.05f;
    cursor->callback = collision_callback;
    cursor->callback_data = (void *) ((long) 0);

    RigidBody *ball1  = physics_scene_add(&physics_scene);
    ball1->position.x = 0.0f;
    ball1->position.y = 0.0f;
    ball1->collider_type = COLLIDER_CIRCLE;
    ball1->collider.circle.radius = 0.1f;
    ball1->callback = collision_callback;
    ball1->callback_data = (void *) ((long) 1);

    RigidBody *ball2  = physics_scene_add(&physics_scene);
    ball2->position.x =-0.4f;
    ball2->position.y = 0.0f;
    ball2->collider_type = COLLIDER_CIRCLE;
    ball2->collider.circle.radius = 0.15f;
    ball2->callback = collision_callback;
    ball2->callback_data = (void *) ((long) 2);

    RigidBody *dude  = physics_scene_add(&physics_scene);
    dude->position.x =-0.4f;
    dude->position.y =-0.4f;
    dude->collider_type = COLLIDER_RECT;
    dude->collider.rect.width = 0.3f;
    dude->collider.rect.height = 0.3f;
    dude->callback = collision_callback;
    dude->callback_data = (void *) ((long) 3);

    while (1) {

        window_update(&window);
        if (window.input.quit) break;

        catalog_service(&catalog); // Make sure we hotload stuff.

        // Adjust mouse position.
        int mouse_x = window.input.mouse_x + (dc.width - window.width)/2;
        if      (mouse_x < 0)           mouse_x = 0;
        else if (mouse_x > dc.width-1)  mouse_x = dc.width-1;
        int mouse_y = window.input.mouse_y + (dc.height - window.height)/2;
        if      (mouse_y < 0)           mouse_y = 0;
        else if (mouse_y > dc.height-1) mouse_y = dc.height-1;

        float x = (float) mouse_x / dc.width  * 2.0f - 1.0f;
        float y = (float) mouse_y / dc.height * 2.0f - 1.0f;
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
        snprintf(buffer, 64, "(%1.2f, %1.2f)", x, y);
        draw_set_color(&dc, 0.1f, 0.1f, 0.1f, 1.0f);
        draw_string(&dc, &font, buffer, 0.5f + 0.01f, -0.9f + 0.01f, TEXT_ALIGN_CENTER);
        draw_set_color(&dc, 0.1f, 0.8f, 0.9f, 1.0f);
        draw_string(&dc, &font, buffer, 0.5f, -0.9f, TEXT_ALIGN_CENTER);

        // Sprite of man.
        state = (window.elapsed_ms / 100) % 4;
        Sprite sprite;
        sprite_init(&sprite, dude->position.x, dude->position.y,
                             dude->collider.rect.width, dude->collider.rect.height,
                             state*0.25f, 0.0f, state*0.25f + 0.25f, 1.0f);
        draw_sprite(&dc, &dude_texture, &sprite);

        // TODO Rigid bodies!

        const float acceleration = 2.0f;
        cursor->velocity.x -= window.input.left  * acceleration*window.dt;
        cursor->velocity.x += window.input.right * acceleration*window.dt;
        cursor->velocity.y -= window.input.down  * acceleration*window.dt;
        cursor->velocity.y += window.input.up    * acceleration*window.dt;

        physics_scene_step(&physics_scene, window.dt);

        draw_set_color(&dc, 0.2f, 0.3f, 0.7f, 1.0f);
        draw_circle(&dc, cursor->position.x, cursor->position.y, cursor->collider.circle.radius);

        draw_set_color(&dc, 0.6f, 0.1f, 0.1f, 1.0f);
        draw_circle(&dc, ball1->position.x, ball1->position.y, ball1->collider.circle.radius);

        draw_set_color(&dc, 0.2f, 0.8f, 0.2f, 1.0f);
        draw_circle(&dc, ball2->position.x, ball2->position.y, ball2->collider.circle.radius);

        window_redraw(&window);

    }

    physics_scene_destroy(&physics_scene);

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
