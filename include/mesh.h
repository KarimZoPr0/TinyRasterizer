//
// Created by Karim on 2023-10-08.
//

#ifndef INC_3D_GRAPHICS_MESH_H
#define INC_3D_GRAPHICS_MESH_H

#include "vector.h"
#include "triangle.h"

#define N_MESH_VERTICES 8
extern vec3_t cube_vertices[N_MESH_VERTICES];

#define N_MESH_FACES (6 * 2)
extern face_t cube_faces[N_MESH_FACES];

typedef struct
{
    vec3_t *vertices;   // Dynamic array of vertices
    face_t *faces;      // Dynamic array of faces
    vec3_t rotation;    // Rotations with x,y and z values
} mesh_t;



void load_cube_mesh_data();
void load_obj_file_data(mesh_t *mesh, char *filename);

#endif //INC_3D_GRAPHICS_MESH_H
