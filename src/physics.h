#ifndef PHYSICS_H
#define PHYSICS_H

#include "vector_math.h"
#include "array.h"

// Colliders. TODO Add ones other than circles.

enum ColliderType {
    COLLIDER_CIRCLE    = 0,
    COLLIDER_RECT      = 1,
    COLLIDER_UNDEFINED = 2
};
typedef enum ColliderType ColliderType;

struct CircleCollider {
    float radius;
};
typedef struct CircleCollider CircleCollider;

struct RectCollider {
    float x, y,
          width, height;
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
    } collider;

    void (*callback)(void *a_data, void *b_data);
    void *callback_data;

};
typedef struct RigidBody RigidBody;

// Physics scene.

struct PhysicsScene {
    Array rigid_bodies;
    Array rigid_bodies_backup;
};
typedef struct PhysicsScene PhysicsScene;

void physics_scene_init(PhysicsScene *scene);
void physics_scene_destroy(PhysicsScene *scene);
RigidBody *physics_scene_add(PhysicsScene *scene);

void physics_scene_step(PhysicsScene *scene, float dt);

#endif
