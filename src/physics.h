#ifndef PHYSICS_H
#define PHYSICS_H

#include "array.h"

// Vector2.

struct Vector2 {
    float x, y;
};
typedef struct Vector2 Vector2;

Vector2 vector2_add(const Vector2 *a, const Vector2 *b);
Vector2 vector2_sub(const Vector2 *a, const Vector2 *b);
Vector2 vector2_mul(const Vector2 *v, float s);
Vector2 vector2_div(const Vector2 *v, float s);

float vector2_length(const Vector2 *v);
float vector2_length_squared(const Vector2 *v);

Vector2 vector2_normalize(const Vector2 *v);

float vector2_dot(const Vector2 *a, const Vector2 *b);

// Colliders. TODO Add ones other than circles.

enum ColliderType {
    COLLIDER_UNDEFINED,
    COLLIDER_CIRCLE
};
typedef enum ColliderType ColliderType;

struct CircleCollider {
    float radius;
};
typedef struct CircleCollider CircleCollider;

// Rigid body.

#define RIGID_BODY_STATIC 1

struct RigidBody {

    Vector2 position,
            velocity;

    int flags;

    ColliderType collider_type;
    union {
        CircleCollider circle;
    } collider;

    void (*callback)(void *a_data, void *b_data);
    void *callback_data;

};
typedef struct RigidBody RigidBody;

void rigid_body_init(RigidBody *body, int flags);

void rigid_bodies_step(Array *array, float dt);

#endif
