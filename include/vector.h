//
// Created by Karim on 2023-10-08.
//

#ifndef INC_3D_GRAPHICS_VECTOR_H
#define INC_3D_GRAPHICS_VECTOR_H

typedef struct
{
    float x;
    float y;
} vec2_t;

typedef struct
{
    float x;
    float y;
    float z;
} vec3_t;

vec3_t vec3_rotate_x( vec3_t v, float angle );
vec3_t vec3_rotate_y( vec3_t v, float angle );
vec3_t vec3_rotate_z( vec3_t v, float angle );

#endif //INC_3D_GRAPHICS_VECTOR_H
