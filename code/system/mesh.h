//
// Created by Karim on 2023-10-08.
//

#ifndef INC_3D_GRAPHICS_MESH_H
#define INC_3D_GRAPHICS_MESH_H

typedef struct face_t face_t;
struct face_t
{
    U32 a;
    U32 b;
    U32 c;
    U32 color;
};

typedef struct triangle_t triangle_t;
struct triangle_t
{
    vec2_t points[3];
    U32 color;
    F32 avg_depth;
};

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
    U64 capacity;
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
    U32 capacity;
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

    // file info
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

typedef struct game_state_t game_state_t;
struct game_state_t;

function vec3_t get_vertex_by_index(vertex_chunk_list_t* vertex_chunks, U64 idx);
function mesh_t* mesh_from_key(game_state_t *state, char* filename);

#define N_MESH_VERTICES 8
extern vec3_t cube_vertices[N_MESH_VERTICES];

#define N_MESH_FACES (6 * 2)
extern face_t cube_faces[N_MESH_FACES];

#endif //INC_3D_GRAPHICS_MESH_H
