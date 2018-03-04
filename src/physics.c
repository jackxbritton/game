#include "physics.h"
#include "misc.h"
#include <math.h>

// Vector2 stuff.

Vector2 vector2_add(const Vector2 *a, const Vector2 *b) {
    Vector2 out;
    out.x = a->x + b->x;
    out.y = a->y + b->y;
    return out;
}
Vector2 vector2_sub(const Vector2 *a, const Vector2 *b) {
    Vector2 out;
    out.x = a->x - b->x;
    out.y = a->y - b->y;
    return out;
}
Vector2 vector2_mul(const Vector2 *v, float s) {
    Vector2 out;
    out.x = v->x * s;
    out.y = v->y * s;
    return out;
}
Vector2 vector2_div(const Vector2 *v, float s) {
    Vector2 out;
    out.x = v->x / s;
    out.y = v->y / s;
    return out;
}

float vector2_length(const Vector2 *v) {
    return sqrtf(v->x*v->x + v->y*v->y);
}

float vector2_length_squared(const Vector2 *v) {
    return v->x*v->x + v->y*v->y;
}

Vector2 vector2_normalize(const Vector2 *v) {
    Vector2 out;
    float l = sqrtf(v->x*v->x + v->y*v->y);
    if (l == 0.0f) {
        out.x = 0.0f;
        out.y = 0.0f;
    } else {
        out.x = v->x / l;
        out.y = v->y / l;
    }
    return out;
}

float vector2_dot(const Vector2 *a, const Vector2 *b) {
    return a->x*b->x + a->y*b->y;
}

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
        RigidBody *rb = array_get(array, i);
        rb->position.x += rb->velocity.x * dt;
        rb->position.y += rb->velocity.y * dt;
    }

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
                    impulse.x = contact_normal.x*contact_speed;
                    impulse.y = contact_normal.y*contact_speed;
                }

            } else {
                DEBUG("Trying to update rigid bodies with collider_type COLLIDER_UNDEFINED!");
                return;
            }

            // Make callbacks.
            if (a->callback != NULL) a->callback(a->callback_data, b->callback_data);
            if (b->callback != NULL) b->callback(b->callback_data, a->callback_data);

            // Apply the impulse vectors.
            a->velocity.x -= impulse.x/2.0f;
            a->velocity.y -= impulse.y/2.0f;
            b->velocity.x += impulse.x/2.0f;
            b->velocity.y += impulse.y/2.0f;

        }
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
