//
// Created by Karim on 2023-10-08.
//

#ifndef INC_3D_GRAPHICS_MESH_H
#define INC_3D_GRAPHICS_MESH_H

#define N_MESH_VERTICES 8
extern vec3_t cube_vertices[N_MESH_VERTICES];

#define N_MESH_FACES (6 * 2)
extern face_t cube_faces[N_MESH_FACES];

//- karim: vertex types
typedef struct vertex_node_t vertex_node_t;
struct vertex_node_t
{
    vertex_node_t *next;
    vertex_node_t *prev;
    vec3_t v;
};

typedef struct vertex_chunk_node_t vertex_chunk_node_t;
struct vertex_chunk_node_t
{
    vertex_chunk_node_t *next;
    vec3_t *v;
    U64 cap;
    U64 count;
};

typedef struct vertex_chunk_list_t vertex_chunk_list_t;
struct vertex_chunk_list_t
{
    vertex_chunk_node_t *first;
    vertex_chunk_node_t *last;
    U64 chunk_count;
    U64 total_count;
};

typedef struct vertex_array_t vertex_array_t;
struct vertex_array_t
{
    vec3_t *v;
    U64 count;
};

//- karim: face types
typedef struct face_node_t face_node_t;
struct face_node_t
{
    face_node_t *next;
    face_t v;
};

typedef struct face_chunk_node_t face_chunk_node_t;
struct face_chunk_node_t
{
    face_chunk_node_t *next;
    face_t *v;
    U64 cap;
    U64 count;
};

typedef struct face_chunk_list_t face_chunk_list_t;
struct face_chunk_list_t
{
    face_chunk_node_t *first;
    face_chunk_node_t *last;
    U64 chunk_count;
    U64 total_count;
};

typedef struct face_array_t face_array_t;
struct face_array_t
{
    face_t *v;
    U64 count;
};

//- karim: mesh type
typedef struct mesh_t mesh_t;
struct mesh_t
{
    // face
    face_chunk_list_t face_chunks;
    face_array_t face_array;

    // vertex
    vertex_chunk_list_t vertex_chunks;
    vertex_array_t vertex_array;

    vec3_t rotation;

    // file properties
    char *filename;
    FILETIME last_write_time;
};

typedef struct mesh_node_t mesh_node_t;
struct mesh_node_t
{
    mesh_node_t *next;
    mesh_t v;
};

typedef struct mesh_slot_t mesh_slot_t;
struct mesh_slot_t
{
    mesh_node_t *first;
    mesh_node_t *last;
};

typedef struct mesh_table_t mesh_table_t;
struct mesh_table_t
{
    mesh_slot_t *slots;
    U64 slot_count;
};

void init_nil_mesh(arena_t* arena, mesh_t* mesh);
mesh_t* mesh_from_key(arena_t* arena, mesh_table_t* table, char* filename);

#endif //INC_3D_GRAPHICS_MESH_H
