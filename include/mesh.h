//
// Created by Karim on 2023-10-08.
//

#ifndef INC_3D_GRAPHICS_MESH_H
#define INC_3D_GRAPHICS_MESH_H

#include "vector.h"
#include "triangle.h"

#define N_MESH_VERTICES 8
extern vec3_t mesh_vertices[N_MESH_VERTICES];

#define N_MESH_FACES (6 * 2)
extern face_t mesh_faces[N_MESH_FACES];

#endif //INC_3D_GRAPHICS_MESH_H
