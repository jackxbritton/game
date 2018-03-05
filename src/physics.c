#include "physics.h"
#include "misc.h"
#include <math.h>

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

static int collision_test(Array *array) {

    for (int i = 0; i < array->count; i++) {
        for (int j = 0; j < i; j++) {

            RigidBody *a = array_get(array, i);
            RigidBody *b = array_get(array, j);

            if (a->collider_type == COLLIDER_CIRCLE && b->collider_type == COLLIDER_CIRCLE) {

                const float ar = a->collider.circle.radius;
                const float br = b->collider.circle.radius;

                const Vector2 diff = vector2_sub(a->position, b->position);
                const float diff_len = vector2_length(diff);
                const float resolution_len = ar + br - diff_len;
                if (resolution_len > 0.0f) return 1; // There was a collision.

                //const Vector2 contact_normal = vector2_normalize(diff);
                //*resolution = vector2_mul(contact_normal, resolution_len);
                

            } else {
                DEBUG("Trying to update rigid bodies with collider_type COLLIDER_UNDEFINED!");
                return 0;
            }

        }
    }

    return 0;
}

static void impulse_resolution(Array *array) {

    for (int i = 0; i < array->count; i++) {
        for (int j = 0; j < i; j++) {

            RigidBody *a = array_get(array, i);
            RigidBody *b = array_get(array, j);

            Vector2 a_resolution,
                    b_resolution,
                    a_impulse,
                    b_impulse;

            if (a->collider_type == COLLIDER_CIRCLE && b->collider_type == COLLIDER_CIRCLE) {

                const float ar = a->collider.circle.radius;
                const float br = b->collider.circle.radius;

                const Vector2 diff = vector2_sub(a->position, b->position);
                const float diff_len = vector2_length(diff);
                const float resolution_len = ar + br - diff_len;
                if (resolution_len <= 0.0f) continue; // No collision here.

                const Vector2 contact_normal = vector2_normalize(diff);
                const Vector2 resolution = vector2_mul(contact_normal, resolution_len);

                a_resolution = vector2_mul(resolution, 0.5f);
                b_resolution = vector2_mul(resolution,-0.5f);

                const Vector2 relative_velocity = vector2_sub(a->velocity, b->velocity);
                const float contact_speed = vector2_dot(contact_normal, relative_velocity);

                a_impulse = vector2_mul(contact_normal,-0.5f*contact_speed);
                b_impulse = vector2_mul(contact_normal, 0.5f*contact_speed);

            } else {
                DEBUG("Trying to update rigid bodies with collider_type COLLIDER_UNDEFINED!");
                return;
            }

            a->position = vector2_add(a->position, a_resolution);
            b->position = vector2_add(b->position, b_resolution);
            a->velocity = vector2_add(a->velocity, a_impulse);
            b->velocity = vector2_add(b->velocity, b_impulse);

            DEBUG("%f", vector2_length(a->velocity));

        }
    }
}

void physics_scene_step(PhysicsScene *scene, float dt) {

    // TODO Broadphase.
    // TODO Binary search through dt for sub-frame collision detection.
    // TODO Iterate over collision detection a few times.

    // Operate on the backup array while we hunt for collisions.
    Array *array = &scene->rigid_bodies_backup;
    array_copy(array, &scene->rigid_bodies); // Update the backup.

    update_positions(array, dt);

    if (!collision_test(array)) {
        // There was no collision, move along.
        array_copy(&scene->rigid_bodies, array);
        return;
    }

    // Otherwise, begin binary search.
    float guess = 0.5f * dt,
          p     = 0.25f;
    for (int i = 0; i < 16; i++) {

        array_copy(array, &scene->rigid_bodies); // Restore the backup.
        update_positions(array, guess);

        if (collision_test(array)) guess -= p*dt;
        else                       guess += p*dt;
        p /= 2.0f;

    }

    // TODO Adjust velocities and junk.

    // TODO Resolve impulses again?
    
    impulse_resolution(array);

    array_copy(&scene->rigid_bodies, array);

    if (dt-guess > 0.0f) physics_scene_step(scene, dt - guess); // Update again.

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
