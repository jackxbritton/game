#ifndef PHYSICS_H
#define PHYSICS_H

#include "vector_math.h"
#include "array.h"

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
