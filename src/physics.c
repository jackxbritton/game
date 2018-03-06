#include "physics.h"
#include "misc.h"
#include <math.h>

/* TODO

   LONG TERM
   broadphase (quadtrees?)

   SHORT TERM
   Jump table.
   Rect collider.

*/

// Collision test functions and jump table.

static int collision_test_circle_circle(RigidBody *a, RigidBody *b) {

    const float ar = a->collider.circle.radius;
    const float br = b->collider.circle.radius;

    const Vector2 diff = vector2_sub(a->position, b->position);
    const float diff_len = vector2_length(diff);
    const float resolution_len = ar + br - diff_len;
    if (resolution_len > 0.0f) return 1; // There was a collision.
    return 0;
}

static int collision_test_circle_rect(RigidBody *a, RigidBody *b) {
    // TODO
}
static int collision_test_rect_circle(RigidBody *a, RigidBody *b) {
    return collision_test_circle_rect(b, a);
}

// TODO
static int collision_test_rect_rect(RigidBody *a, RigidBody *b) { return 0; }

// To look up a collision test function, use collision_test_table[a_type][b_type].
static int (*collision_test_table[][COLLIDER_UNDEFINED])(RigidBody *, RigidBody *) = {
    { collision_test_circle_circle, collision_test_circle_rect },
    { collision_test_rect_circle,   collision_test_rect_rect }
};

static int collision_test(Array *array) {
    for (int i = 0; i < array->count; i++) {
        for (int j = 0; j < i; j++) {
            RigidBody *a = array_get(array, i);
            RigidBody *b = array_get(array, j);
            if (collision_test_table[a->collider_type][b->collider_type](a, b)) return 1;
        }
    }
    return 0;
}

// Collision resolution functions and jump table.

static int collision_resolve_circle_circle(RigidBody *a, RigidBody *b) {

    const float ar = a->collider.circle.radius;
    const float br = b->collider.circle.radius;

    const Vector2 diff = vector2_sub(a->position, b->position);
    const float diff_len = vector2_length(diff);
    const float resolution_len = ar + br - diff_len;
    if (resolution_len <= 0.0f) return 0; // No collision here.

    const Vector2 contact_normal = vector2_normalize(diff);
    const Vector2 resolution = vector2_mul(contact_normal, resolution_len);

    a->position = vector2_add(a->position, vector2_mul(resolution, 0.5f));
    b->position = vector2_add(b->position, vector2_mul(resolution, -0.5f));

    const Vector2 relative_velocity = vector2_sub(a->velocity, b->velocity);
    const float contact_speed = vector2_dot(contact_normal, relative_velocity);

    const float elasticity = 1.0f;
    a->velocity = vector2_add(a->velocity,
                              vector2_mul(contact_normal,-0.5f*contact_speed*elasticity));
    b->velocity = vector2_add(b->velocity,
                              vector2_mul(contact_normal, 0.5f*contact_speed*elasticity));

    return 1;

}

static int collision_resolve_circle_rect(RigidBody *a, RigidBody *b) {
    // TODO
}

static int collision_resolve_rect_circle(RigidBody *a, RigidBody *b) {
    return collision_resolve_circle_rect(b, a);
}

// TODO
static int collision_resolve_rect_rect(RigidBody *a, RigidBody *b) { return 0; }

// To look up a collision resolution function, use collision_test_table[a_type][b_type].
static int (*collision_resolution_table[][COLLIDER_UNDEFINED])(RigidBody *, RigidBody *) = {
    { collision_resolve_circle_circle, collision_resolve_circle_rect },
    { collision_resolve_rect_circle,   collision_resolve_rect_rect }
};

static void update_positions(Array *array, float dt) {

    // Position = velocity * time.
    for (int i = 0; i < array->count; i++) {

        RigidBody *rb = array_get(array, i);
        rb->position.x += rb->velocity.x * dt;
        rb->position.y += rb->velocity.y * dt;

        // Friction.
        const float friction = 0.7f;
        Vector2 norm = vector2_normalize(rb->velocity);
        float speed = vector2_length(rb->velocity) - friction*dt;
        if (speed < 0.0f) speed = 0.0f;
        rb->velocity = vector2_mul(norm, speed);
    }

}

static void impulse_resolution(Array *array) {
    for (int i = 0; i < array->count; i++) {
        for (int j = 0; j < i; j++) {
            RigidBody *a = array_get(array, i);
            RigidBody *b = array_get(array, j);
            if (collision_resolution_table[a->collider_type][b->collider_type](a, b)) {
                // Make function callbacks.
                if (a->callback != NULL) a->callback(a->callback_data, b->callback_data);
                if (b->callback != NULL) b->callback(b->callback_data, a->callback_data);
            }
        }
    }
}

void physics_scene_step(PhysicsScene *scene, float dt) {

    // TODO Broadphase.

    for (int n = 0; n < 128; n++) {

        // Operate on the backup array while we hunt for collisions.
        Array *array = &scene->rigid_bodies_backup;
        array_copy(array, &scene->rigid_bodies); // Update the backup.

        update_positions(array, dt);

        if (!collision_test(array)) {
            // There was no collision so we're done!
            array_copy(&scene->rigid_bodies, array);
            break;
        }

        // Otherwise, begin binary search for the time of collision.
        float guess = dt,
              p     = 1.0f;
        for (int i = 0; i < 16; i++) {

            p /= 2.0f;
            if (collision_test(array)) guess -= p*dt;
            else                       guess += p*dt;

            array_copy(array, &scene->rigid_bodies); // Restore the backup.
            update_positions(array, guess);

        }
        
        impulse_resolution(array);

        array_copy(&scene->rigid_bodies, array);

        dt -= guess;
        if (dt > 0.0f) continue;
        else           break;

    }

}

void physics_scene_init(PhysicsScene *scene) {
    array_init(&scene->rigid_bodies, 16, sizeof(RigidBody));
    array_clone(&scene->rigid_bodies_backup, &scene->rigid_bodies);
}

void physics_scene_destroy(PhysicsScene *scene) {
    array_destroy(&scene->rigid_bodies);
    array_destroy(&scene->rigid_bodies_backup);
}

RigidBody *physics_scene_add(PhysicsScene *scene) {

    array_add(&scene->rigid_bodies, NULL);
    RigidBody *body = array_get(&scene->rigid_bodies, scene->rigid_bodies.count-1);

    body->flags = 0;
    body->collider_type = COLLIDER_UNDEFINED;

    body->callback = NULL;
    body->callback_data = NULL;

    body->position.x = 0.0f;
    body->position.y = 0.0f;
    body->velocity.x = 0.0f;
    body->velocity.y = 0.0f;

    return body;
}
