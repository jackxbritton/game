#include "physics.h"
#include "misc.h"
#include <math.h>

// Vector2 stuff.

float vector2_length(Vector2 *v) {
    return sqrtf(v->x*v->x + v->y*v->y);
}

float vector2_length_squared(Vector2 *v) {
    return v->x*v->x + v->y*v->y;
}

void vector2_normalize(Vector2 *v) {
    float l = sqrtf(v->x*v->x + v->y*v->y);
    if (l == 0.0f) {
        v->x = 0.0f;
        v->y = 0.0f;
    } else {
        v->x /= l;
        v->y /= l;
    }
}

// Rigid body stuff.

void rigid_body_init(RigidBody *body, int flags) {
    body->flags = flags;
    body->collider_type = COLLIDER_UNDEFINED;
    body->position.x = 0.0f;
    body->position.y = 0.0f;
    body->velocity.x = 0.0f;
    body->velocity.y = 0.0f;
}

static int collision_test_circle_circle(CircleCollider *c1, CircleCollider *c2) {
    return 0;
}

static int collision_test_circle_rect(CircleCollider *c, RectCollider *r) {
    return 0;
}

static int collision_test_rect_rect(RectCollider *r1, RectCollider *r2) {
    return 0;
}

static void update(Array *array, float dt) {

    // TODO How can the user be notified when a rigid body they care about
    // has a collision? Callback functions, OR this function halts
    // and you have to run it repeatedly to completion.

    // TODO Binary search for sub-frame collision detection.

    for (int i = 0; i < array->count; i++) {
        RigidBody *rb = array_get(array, i);
        rb->position.x += rb->velocity.x * dt;
        rb->position.y += rb->velocity.y * dt;
    }

    for (int i = 0; i < array->count; i++) {
        for (int j = 0; j < i; j++) {

            RigidBody *a = array_get(array, i);
            RigidBody *b = array_get(array, j);

            int collision = 0;

            if (a->collider_type == COLLIDER_CIRCLE || b->collider_type == COLLIDER_CIRCLE) {
                collision = collision_test_circle_circle(&a->collider.circle, &b->collider.circle);
            } else if (a->collider_type == COLLIDER_CIRCLE || b->collider_type == COLLIDER_RECT) {
                collision = collision_test_circle_rect(&a->collider.circle, &b->collider.rect);
            } else if (a->collider_type == COLLIDER_RECT || b->collider_type == COLLIDER_CIRCLE) {
                collision = collision_test_circle_rect(&b->collider.circle, &a->collider.rect);
            } else if (a->collider_type == COLLIDER_RECT || b->collider_type == COLLIDER_RECT) {
                collision = collision_test_rect_rect(&a->collider.rect, &b->collider.rect);
            } else {
                DEBUG("Trying to update rigid bodies with collider_type COLLIDER_UNDEFINED!");
                return;
            }

            if (collision) {
                DEBUG("there was a collision!");
                // TODO Correction and callbacks.
            }

        }
    }

}

void rigid_bodies_update(Array *array, float dt) {

    const float dt_max = 0.1f; // 10 fps.

    while (dt > dt_max) {
        dt -= dt_max;
        update(array, dt_max);
    }

    update(array, dt);

}
