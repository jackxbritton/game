#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

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

#endif
