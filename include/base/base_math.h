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


vec3_t vec3_rotate_x( vec3_t v, float angle );
vec3_t vec3_rotate_y( vec3_t v, float angle );
vec3_t vec3_rotate_z( vec3_t v, float angle );
