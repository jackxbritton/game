#include "vector_math.h"
#include "misc.h"
#include <math.h>

// Vector2 stuff.

Vector2 vector2_add(Vector2 a, Vector2 b) {
    Vector2 out;
    out.x = a.x + b.x;
    out.y = a.y + b.y;
    return out;
}
Vector2 vector2_sub(Vector2 a, Vector2 b) {
    Vector2 out;
    out.x = a.x - b.x;
    out.y = a.y - b.y;
    return out;
}
Vector2 vector2_mul(Vector2 v, float s) {
    Vector2 out;
    out.x = v.x * s;
    out.y = v.y * s;
    return out;
}
Vector2 vector2_div(Vector2 v, float s) {
    Vector2 out;
    out.x = v.x / s;
    out.y = v.y / s;
    return out;
}

float vector2_length(Vector2 v) {
    return sqrtf(v.x*v.x + v.y*v.y);
}

float vector2_length_squared(Vector2 v) {
    return v.x*v.x + v.y*v.y;
}

Vector2 vector2_normalize(Vector2 v) {
    Vector2 out;
    float l = sqrtf(v.x*v.x + v.y*v.y);
    if (l == 0.0f) {
        out.x = 0.0f;
        out.y = 0.0f;
    } else {
        out.x = v.x / l;
        out.y = v.y / l;
    }
    return out;
}

float vector2_dot(Vector2 a, Vector2 b) {
    return a.x*b.x + a.y*b.y;
}
