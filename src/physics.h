#ifndef PHYSICS_H
#define PHYSICS_H

#include "array.h"

// Vector2.

struct Vector2 {
    float x, y;
};
typedef struct Vector2 Vector2;

float vector2_length(Vector2 *v);
float vector2_length_squared(Vector2 *v);
void vector2_normalize(Vector2 *v);

// Colliders.

enum ColliderType {
    COLLIDER_UNDEFINED,
    COLLIDER_CIRCLE,
    COLLIDER_RECT
};
typedef enum ColliderType ColliderType;

struct CircleCollider {
    float radius;
};
typedef struct CircleCollider CircleCollider;

struct RectCollider {
    float width, height;
};
typedef struct RectCollider RectCollider;

// Rigid body.

#define RIGID_BODY_STATIC 1

struct RigidBody {

    Vector2 position,
            velocity;

    int flags;

    ColliderType collider_type;
    union {
        CircleCollider circle;
        RectCollider rect;
    } collider;

};
typedef struct RigidBody RigidBody;

void rigid_body_init(RigidBody *body, int flags);

void rigid_bodies_update(Array *array, float dt);

#endif
