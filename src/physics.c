#include "physics.h"
#include "misc.h"
#include <math.h>

// Rigid body stuff.

void rigid_body_init(RigidBody *body, int flags) {

    body->flags = flags;
    body->collider_type = COLLIDER_UNDEFINED;

    body->callback = NULL;
    body->callback_data = NULL;

    body->position.x = 0.0f;
    body->position.y = 0.0f;
    body->velocity.x = 0.0f;
    body->velocity.y = 0.0f;

}

static void step(Array *array, float dt) {

    // TODO Binary search through dt for sub-frame collision detection.
    // TODO Iterate over collision detection a few times.

    for (int i = 0; i < array->count; i++) {
        for (int j = 0; j < i; j++) {

            RigidBody *a = array_get(array, i);
            RigidBody *b = array_get(array, j);

            Vector2 impulse;

            if (a->collider_type == COLLIDER_CIRCLE && b->collider_type == COLLIDER_CIRCLE) {

                const float ar = a->collider.circle.radius;
                const float br = b->collider.circle.radius;

                const Vector2 diff = vector2_sub(&a->position, &b->position);
                const float diff_len = vector2_length(&diff);
                if (diff_len > ar + br) continue; // No collision here.

                const Vector2 contact_normal = vector2_normalize(&diff);
                const Vector2 relative_velocity = vector2_sub(&a->velocity, &b->velocity); 
                const float contact_speed = vector2_dot(&relative_velocity, &contact_normal);

                if (contact_speed > 0.0f) {
                    // Nothing to resolve, they're already moving away from each other.
                    impulse.x = 0.0f;
                    impulse.y = 0.0f;
                } else {
                    impulse = vector2_mul(&contact_normal, contact_speed);
                }

            } else {
                DEBUG("Trying to update rigid bodies with collider_type COLLIDER_UNDEFINED!");
                return;
            }

            // Make callbacks.
            if (a->callback != NULL) a->callback(a->callback_data, b->callback_data);
            if (b->callback != NULL) b->callback(b->callback_data, a->callback_data);

            // Apply the impulse vectors.
            Vector2 a_impulse = vector2_div(&impulse, -2.0f);
            a->velocity = vector2_add(&a->velocity, &a_impulse);
            Vector2 b_impulse = vector2_div(&impulse, 2.0f);
            b->velocity = vector2_add(&b->velocity, &b_impulse);

        }
    }

    // Position = velocity * time.
    for (int i = 0; i < array->count; i++) {

        RigidBody *rb = array_get(array, i);
        rb->position.x += rb->velocity.x * dt;
        rb->position.y += rb->velocity.y * dt;

        // Friction.
        const float friction = 0.7f;
        Vector2 norm = vector2_normalize(&rb->velocity);
        float speed = vector2_length(&rb->velocity) - friction*dt;
        if (speed < 0.0f) speed = 0.0f;
        rb->velocity = vector2_mul(&norm, speed);
    }
}

void rigid_bodies_step(Array *array, float dt) {

    const float dt_max = 0.1f; // 10 fps.

    while (dt > dt_max) {
        dt -= dt_max;
        step(array, dt_max);
    }

    step(array, dt);

}
