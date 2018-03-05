#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

// Vector2.

struct Vector2 {
    float x, y;
};
typedef struct Vector2 Vector2;

Vector2 vector2_add(Vector2 a, Vector2 b);
Vector2 vector2_sub(Vector2 a, Vector2 b);
Vector2 vector2_mul(Vector2 v, float s);
Vector2 vector2_div(Vector2 v, float s);

float vector2_length(Vector2 v);
float vector2_length_squared(Vector2 v);

Vector2 vector2_normalize(Vector2 v);

float vector2_dot(Vector2 a, Vector2 b);

#endif
