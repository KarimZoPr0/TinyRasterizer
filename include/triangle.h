//
// Created by Karim on 2023-10-08.
//

#ifndef INC_3D_GRAPHICS_TRIANGLE_H
#define INC_3D_GRAPHICS_TRIANGLE_H

#include "vector.h"

typedef struct
{
    int a;
    int b;
    int c;
} face_t;

typedef struct
{
    vec2_t points[3];
} triangle_t;

#endif //INC_3D_GRAPHICS_TRIANGLE_H
