//
// Created by Karim on 2023-10-08.
//

global mesh_t* nil_mesh = {0};

vec3_t cube_vertices[N_MESH_VERTICES] = {
    {.x = -1, .y = -1, .z = -1}, // 1
    {.x = -1, .y = 1, .z = -1}, // 2
    {.x = 1, .y = 1, .z = -1}, // 3
    {.x = 1, .y = -1, .z = -1}, // 4
    {.x = 1, .y = 1, .z = 1}, // 5
    {.x = 1, .y = -1, .z = 1}, // 6
    {.x = -1, .y = 1, .z = 1}, // 7
    {.x = -1, .y = -1, .z = 1} // 8
};

#define N_MESH_FACES (6 * 2)
face_t cube_faces[N_MESH_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3},
    {.a = 1, .b = 3, .c = 4},
    // right
    {.a = 4, .b = 3, .c = 5},
    {.a = 4, .b = 5, .c = 6},
    // back
    {.a = 6, .b = 5, .c = 7},
    {.a = 6, .b = 7, .c = 8},
    // left
    {.a = 8, .b = 7, .c = 2},
    {.a = 8, .b = 2, .c = 1},
    // top
    {.a = 2, .b = 7, .c = 5},
    {.a = 2, .b = 5, .c = 3},
    // bottom
    {.a = 6, .b = 8, .c = 1},
    {.a = 6, .b = 1, .c = 4}
};

function void init_nil_mesh(arena_t* arena, mesh_t* mesh)
{
    vec3_t* vertices = push_array(arena, vec3_t, N_MESH_VERTICES);
    memcpy(vertices, cube_vertices, sizeof(cube_vertices));
    mesh->vertex_array.v = vertices;

    face_t* faces = push_array(arena, face_t, N_MESH_FACES);
    memcpy(faces, cube_faces, sizeof(cube_faces));
    mesh->face_array.v = faces;

    mesh->vertex_array.count = N_MESH_VERTICES;
    mesh->face_array.count = N_MESH_FACES;
}

function U64 hash_from_string(char* filename)
{
    U64 result = 5435;
    for (U64 i = 0; i < strlen(filename); i++)
    {
        result = ((result << 5) + result) + filename[i];
    }
    return result;
}

function vec3_t get_vertex_by_index(vertex_chunk_list_t* vertex_chunks, U64 idx)
{
    vec3_t vertex = {0};
    U64 remaining = idx - 1;
    for (vertex_chunk_node_t *n = vertex_chunks->first; n != 0; n = n->next)
    {
        if (remaining < n->cap)
        {
            vertex = n->v[remaining];
            break;
        }
        remaining -= n->cap;
    }
    return vertex;
}

function mesh_t* mesh_from_key(arena_t* arena, mesh_table_t* table, char* filename)
{
    arena_t* scratch_arena = scratch_begin();

    //- karim: return value
    mesh_t* existing_mesh = nil_mesh;
    B32 file_changed = 0;

    //- karim: map key -> hash and slot
    U64 hash = hash_from_string(filename);
    U64 slot_idx = hash % table->slot_count;

    //- karim: find existing node in the table
    mesh_slot_t* slot = &table->slots[slot_idx];
    mesh_node_t* existing_node = 0;
    for (mesh_node_t* n = slot->first; n != 0; n = n->next)
    {
        if (strcmp(n->v.filename, filename) == 0)
        {
            existing_mesh = &n->v;
            existing_node = n;
            break;
        }
    }

    if (existing_node)
    {
        /* TODO(karim):
         * hash the file contents and compare it to reload instead of last write time.
         * It's not that comparing last write time is slow, it's the fact that we have to parse the whole file anyways
         * So why not use the contents of file? That way we use the lines buffer for comparing hashes and also parsing!
        */
        FILETIME last_write_time = get_last_write_time(filename);
        if (CompareFileTime(&existing_node->v.last_write_time, &last_write_time) != 0)
        {
            file_changed = 1;
        }
    }

    FILE* file = fopen(filename, "r");
    if (file_changed || (existing_mesh == nil_mesh && file))
    {
        //- karim: allocate a new mesh or reuse an existing one
        existing_mesh = push_array(arena, mesh_t, 1);

        //- karim: parse the file
        // TODO(karim): load the whole file contents into a buffer
        char line[1024];
        while (fgets(line, sizeof(line), file))
        {
            if (strncmp(line, "v ", 2) == 0)
            {
                //- karim: vertex chunked linked-list
                vertex_chunk_node_t* n = existing_mesh->vertex_chunks.last;
                if (n == 0 || n->count >= n->cap)
                {
                    // TODO(karim): create a vertex chunk free list
                    n = push_array(arena, vertex_chunk_node_t, 1);
                    SLLQueuePush(existing_mesh->vertex_chunks.first, existing_mesh->vertex_chunks.last, n);
                    n->cap = 256;
                    n->v = push_array(arena, vec3_t, n->cap);
                    existing_mesh->vertex_chunks.chunk_count++;
                }
                vec3_t* vertex = &n->v[n->count++];
                sscanf_s(line, "v %f %f %f", &vertex->x, &vertex->y, &vertex->z);
                existing_mesh->vertex_chunks.total_count++;
            }
            else if (strncmp(line, "f ", 2) == 0)
            {
                //- karim: parse face
                face_chunk_node_t* n = existing_mesh->face_chunks.last;
                if (n == 0 || n->count >= n->cap)
                {
                    // TODO(karim): create a face chunk free list
                    n = push_array(arena, face_chunk_node_t, 1);
                    SLLQueuePush(existing_mesh->face_chunks.first, existing_mesh->face_chunks.last, n);
                    n->cap = 256;
                    n->v = push_array(arena, face_t, n->cap);
                    existing_mesh->face_chunks.chunk_count++;
                }
                face_t* face = &n->v[n->count++];
                U32 vertex_indices[3], texture_indices[3], normal_indices[3];
                sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                         &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                         &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                         &vertex_indices[2], &texture_indices[2], &normal_indices[2]);
                *face = (face_t){vertex_indices[0], vertex_indices[1], vertex_indices[2]};
                existing_mesh->face_chunks.total_count++;
            }
        }

        //- karim: update or insert the node
        if (!existing_node)
        {
            mesh_node_t* n = push_array(arena, mesh_node_t, 1);
            n->v = *existing_mesh;
            n->v.filename = filename;
            n->v.last_write_time = get_last_write_time(filename);
            SLLQueuePush(slot->first, slot->last, n);
        }
        else
        {
            existing_node->v = *existing_mesh;
            existing_node->v.last_write_time = get_last_write_time(filename);
            existing_node->v.filename = filename;
        }
    }

    if (file) fclose(file);
    scratch_end(scratch_arena);
    return existing_mesh;
}
