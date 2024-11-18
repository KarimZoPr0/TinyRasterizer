//
// Created by Karim on 2024-11-12.
//

#ifndef BASE_MATH_C_H
#define BASE_MATH_C_H

#endif //BASE_MATH_C_H

//~ karim: Vector-Types
typedef struct
{
    F32 x;
    F32 y;
} vec2_t;

typedef struct
{
    F32 x;
    F32 y;
    F32 z;
} vec3_t;

typedef struct
{
    U32 a;
    U32 b;
    U32 c;
} face_t;

typedef struct
{
    vec2_t points[3];
} triangle_t;

typedef struct vec4_t
{
    F32 x, y, z, h;

} vec4_t;

typedef struct
{
    F32 x, y, w, h;

} rect_t;

// Vector 2D functions
float vec2_length(vec2_t v);
vec2_t vec2_add(vec2_t a, vec2_t b);
vec2_t vec2_sub(vec2_t a, vec2_t b);
vec2_t vec2_mul(vec2_t a, F32 factor);
vec2_t vec2_div(vec2_t a, F32 factor);
float vec2_dot(vec2_t a, vec2_t b);


// Vector 3D functions
float vec3_length(vec3_t v);
vec3_t vec3_add(vec3_t a, vec3_t b);
vec3_t vec3_sub(vec3_t a, vec3_t b);
vec3_t vec3_mul(vec3_t a, F32 factor);
vec3_t vec3_div(vec3_t a, F32 factor);
vec3_t vec3_cross(vec3_t a, vec3_t b);
float vec3_dot(vec3_t a, vec3_t b);

vec3_t vec3_rotate_x( vec3_t v, float angle );
vec3_t vec3_rotate_y( vec3_t v, float angle );
vec3_t vec3_rotate_z( vec3_t v, float angle );
