//
// Created by Karim on 2023-10-08.
//

vec3_t cube_vertices[N_CUBE_VERTICES] = {
    {.x = -1, .y = -1, .z = -1}, // 1
    {.x = -1, .y = 1, .z = -1}, // 2
    {.x = 1, .y = 1, .z = -1}, // 3
    {.x = 1, .y = -1, .z = -1}, // 4
    {.x = 1, .y = 1, .z = 1}, // 5
    {.x = 1, .y = -1, .z = 1}, // 6
    {.x = -1, .y = 1, .z = 1}, // 7
    {.x = -1, .y = -1, .z = 1} // 8
};

face_t cube_faces[N_CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3, .a_uv = {0, 0}, .b_uv = {0, 1}, .c_uv = {1, 1}, .color = 0xFFFFFFFF},
    {.a = 1, .b = 3, .c = 4, .a_uv = {0, 0}, .b_uv = {1, 1}, .c_uv = {1, 0}, .color = 0xFFFFFFFF},
    // right
    {.a = 4, .b = 3, .c = 5, .a_uv = {0, 0}, .b_uv = {0, 1}, .c_uv = {1, 1}, .color = 0xFFFFFFFF},
    {.a = 4, .b = 5, .c = 6, .a_uv = {0, 0}, .b_uv = {1, 1}, .c_uv = {1, 0}, .color = 0xFFFFFFFF},
    // back
    {.a = 6, .b = 5, .c = 7, .a_uv = {0, 0}, .b_uv = {0, 1}, .c_uv = {1, 1}, .color = 0xFFFFFFFF},
    {.a = 6, .b = 7, .c = 8, .a_uv = {0, 0}, .b_uv = {1, 1}, .c_uv = {1, 0}, .color = 0xFFFFFFFF},
    // left
    {.a = 8, .b = 7, .c = 2, .a_uv = {0, 0}, .b_uv = {0, 1}, .c_uv = {1, 1}, .color = 0xFFFFFFFF},
    {.a = 8, .b = 2, .c = 1, .a_uv = {0, 0}, .b_uv = {1, 1}, .c_uv = {1, 0}, .color = 0xFFFFFFFF},
    // top
    {.a = 2, .b = 7, .c = 5, .a_uv = {0, 0}, .b_uv = {0, 1}, .c_uv = {1, 1}, .color = 0xFFFFFFFF},
    {.a = 2, .b = 5, .c = 3, .a_uv = {0, 0}, .b_uv = {1, 1}, .c_uv = {1, 0}, .color = 0xFFFFFFFF},
    // bottom
    {.a = 6, .b = 8, .c = 1, .a_uv = {0, 0}, .b_uv = {0, 1}, .c_uv = {1, 1}, .color = 0xFFFFFFFF},
    {.a = 6, .b = 1, .c = 4, .a_uv = {0, 0}, .b_uv = {1, 1}, .c_uv = {1, 0}, .color = 0xFFFFFFFF}
};

function U64 hash_from_string(char *filename)
{
    U64 result = 5435;
    for (U64 i = 0; i < strlen(filename); i++)
    {
        result = ((result << 5) + result) + filename[i];
    }
    return result;
}

function vec3_t get_vertex_by_index(vertex_chunk_list_t *vertex_chunks, U64 idx)
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

function mesh_t *mesh_from_key_new(game_state_t *state, char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == 0) return state->nil_entity.mesh;

    temp_t scratch = scratch_begin(0, 0);

    //- karim: return value
    mesh_t *existing_mesh = 0;

    //- karim: map key -> hash and slot
    U64 hash = hash_from_string(filename);
    U64 slot_idx = hash & state->mesh_table->slot_count - 1;

    //- karim: find existing node in the table
    mesh_slot_t *slot = &state->mesh_table->slots[slot_idx];
    mesh_node_t *existing_node = 0;
    for (mesh_node_t *n = slot->first; n != 0; n = n->next)
    {
        if (strcmp(n->v.filename, filename) == 0)
        {
            existing_mesh = &n->v;
            existing_node = n;
            break;
        }
    }

    //- karim: Existing node not found? -> Allocate node
    if (existing_node == 0)
    {
        existing_node = push_array(state->meshes_arena, mesh_node_t, 1);
        existing_mesh = &existing_node->v;
        existing_mesh->arena = arena_create();
        existing_mesh->filename = filename;
        SLLQueuePush(slot->first, slot->last, existing_node);
    }

    //- karim: has file changed
    FILETIME last_write_time = get_last_write_time(filename);
    B32 file_changed = CompareFileTime(&existing_node->v.last_write_time, &last_write_time) != 0;
    if (file_changed && file)
    {
        //- karim: prepare the mesh
        arena_clear(existing_mesh->arena);
        MemoryZeroStruct(&existing_mesh->vertex_chunks);
        MemoryZeroStruct(&existing_mesh->face_chunks);

        //- karim: parse the file
        char line[1024];
        while (fgets(line, sizeof(line), file))
        {
            //- karim: vertex chunked linked-list
            if (strncmp(line, "v ", 2) == 0)
            {
                vertex_chunk_node_t *n = existing_mesh->vertex_chunks.last;
                if (n == 0 || n->count >= n->cap)
                {
                    n = push_array_no_zero(scratch.arena, vertex_chunk_node_t, 1);
                    n->cap = 1024;
                    n->v = push_array_no_zero(scratch.arena, vec3_t, n->cap);
                    n->next = 0;
                    n->count = 0;
                    SLLQueuePush(existing_mesh->vertex_chunks.first, existing_mesh->vertex_chunks.last, n);
                    existing_mesh->vertex_chunks.chunk_count++;
                }
                vec3_t *vertex = &n->v[n->count++];
                sscanf_s(line, "v %f %f %f", &vertex->x, &vertex->y, &vertex->z);
                existing_mesh->vertex_chunks.total_count++;
            }
            //- karim: face chunked linked-list
            else if (strncmp(line, "f ", 2) == 0)
            {
                face_chunk_node_t *n = existing_mesh->face_chunks.last;
                if (n == 0 || n->count >= n->cap)
                {
                    n = push_array_no_zero(scratch.arena, face_chunk_node_t, 1);
                    n->cap = 1024;
                    n->v = push_array_no_zero(scratch.arena, face_t, n->cap);
                    n->next = 0;
                    n->count = 0;
                    SLLQueuePush(existing_mesh->face_chunks.first, existing_mesh->face_chunks.last, n);
                    existing_mesh->face_chunks.chunk_count++;
                }
                face_t *face = &n->v[n->count++];
                U32 vertex_indices[3], texture_indices[3], normal_indices[3];
                sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                         &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                         &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                         &vertex_indices[2], &texture_indices[2], &normal_indices[2]);
                *face = (face_t){vertex_indices[0], vertex_indices[1], vertex_indices[2]};
                existing_mesh->face_chunks.total_count++;
                face->color = 0xFFFFFFFF;
            }
        }

        existing_mesh->last_write_time = last_write_time;
        //- karim: vertex chunks -> vertex arrays
        {
            existing_mesh->vertex_array.count = existing_mesh->vertex_chunks.total_count;
            existing_mesh->vertex_array.v = push_array(existing_mesh->arena, vec3_t,
                                                       existing_mesh->vertex_array.count);
            U64 idx = 0;
            for (vertex_chunk_node_t *n = existing_mesh->vertex_chunks.first; n != 0; n = n->next)
            {
                MemoryCopy(existing_mesh->vertex_array.v + idx, n->v, n->count * sizeof(vec3_t));
                idx += n->count;
            }
        }

        //- karim: face chunks -> face arrays
        {
            existing_mesh->face_array.count = existing_mesh->face_chunks.total_count;
            existing_mesh->face_array.v = push_array(existing_mesh->arena, face_t, existing_mesh->face_array.count);
            U64 idx = 0;
            for (face_chunk_node_t *n = existing_mesh->face_chunks.first; n != 0; n = n->next)
            {
                MemoryCopy(existing_mesh->face_array.v + idx, n->v, n->count * sizeof(face_t));
                idx += n->count;
            }
        }
    }
    if (file)fclose(file);
    scratch_end(scratch);

    //- karim: We can safely assume there is an existing mesh
    return existing_mesh;
}


function mesh_t *mesh_from_key(game_state_t *state, char *filename)
{
    FILE *file = fopen(filename, "r");
    if (file == 0) return state->nil_entity.mesh;

    //- karim: return value
    mesh_t *existing_mesh = 0;

    B32 file_changed = 0;

    //- karim: map key -> hash and slot
    U64 hash = hash_from_string(filename);
    U64 slot_idx = hash & state->mesh_table->slot_count - 1;

    //- karim: find existing node in the table
    mesh_slot_t *slot = &state->mesh_table->slots[slot_idx];
    mesh_node_t *existing_node = 0;
    for (mesh_node_t *n = slot->first; n != 0; n = n->next)
    {
        if (strcmp(n->v.filename, filename) == 0)
        {
            existing_mesh = &n->v;
            existing_node = n;
            break;
        }
    }

    //- karim: Existing node not found? -> Allocate node
    if (existing_node == 0)
    {
        existing_node = push_array(state->meshes_arena, mesh_node_t, 1);
        existing_mesh = &existing_node->v;
        existing_mesh->filename = filename;
        SLLQueuePush(slot->first, slot->last, existing_node);
    }

    //- karim: has file changed
    FILETIME last_write_time = get_last_write_time(filename);
    file_changed = CompareFileTime(&existing_node->v.last_write_time, &last_write_time) != 0;

    temp_t scratch = scratch_begin(0, 0);
    if (file_changed && file)
    {
        //- karim: free vertex chunks
        if (existing_mesh->vertex_chunks.chunk_count != 0)
        {
            existing_mesh->vertex_chunks.last->next = state->first_free_vertex_chunk;
            state->first_free_vertex_chunk = existing_mesh->vertex_chunks.first;
            MemoryZeroStruct(&existing_mesh->vertex_chunks);
        }

        //- karim: free face chunks
        if (existing_mesh->face_chunks.chunk_count != 0)
        {
            existing_mesh->face_chunks.last->next = state->first_free_face_chunk;
            state->first_free_face_chunk = existing_mesh->face_chunks.first;
            MemoryZeroStruct(&existing_mesh->face_chunks);
        }

        //- karim: parse the file
        char line[1024];
        while (fgets(line, sizeof(line), file))
        {
            //- karim: vertex chunked linked-list
            if (strncmp(line, "v ", 2) == 0)
            {
                vertex_chunk_node_t *n = existing_mesh->vertex_chunks.last;
                if (n == 0 || n->count >= n->cap)
                {
                    n = state->first_free_vertex_chunk;
                    if (n == 0)
                    {
                        n = push_array_no_zero(state->vertex_chunk_arena, vertex_chunk_node_t, 1);
                        n->cap = 1024;
                        n->v = push_array_no_zero(state->vertex_chunk_arena, vec3_t, n->cap);
                    }
                    else
                    {
                        state->first_free_vertex_chunk = state->first_free_vertex_chunk->next;
                    }
                    n->next = 0;
                    n->count = 0;
                    SLLQueuePush(existing_mesh->vertex_chunks.first, existing_mesh->vertex_chunks.last, n);
                    existing_mesh->vertex_chunks.chunk_count++;
                }
                vec3_t *vertex = &n->v[n->count++];
                sscanf_s(line, "v %f %f %f", &vertex->x, &vertex->y, &vertex->z);
                existing_mesh->vertex_chunks.total_count++;
            }
            //- karim: face chunked linked-list
            else if (strncmp(line, "f ", 2) == 0)
            {
                face_chunk_node_t *n = existing_mesh->face_chunks.last;
                if (n == 0 || n->count >= n->cap)
                {
                    n = state->first_free_face_chunk;
                    if (n == 0)
                    {
                        n = push_array_no_zero(state->face_chunk_arena, face_chunk_node_t, 1);
                        n->cap = 1024;
                        n->v = push_array_no_zero(state->face_chunk_arena, face_t, n->cap);
                    }
                    else
                    {
                        state->first_free_face_chunk = state->first_free_face_chunk->next;
                    }
                    n->next = 0;
                    n->count = 0;
                    SLLQueuePush(existing_mesh->face_chunks.first, existing_mesh->face_chunks.last, n);
                    existing_mesh->face_chunks.chunk_count++;
                }
                face_t *face = &n->v[n->count++];
                U32 vertex_indices[3], texture_indices[3], normal_indices[3];
                sscanf_s(line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                         &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                         &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                         &vertex_indices[2], &texture_indices[2], &normal_indices[2]);
                *face = (face_t){vertex_indices[0], vertex_indices[1], vertex_indices[2]};
                existing_mesh->face_chunks.total_count++;
                face->color = 0xFFFFFFFF;
            }
        }
        existing_mesh->last_write_time = last_write_time;
    }


    if (file)fclose(file);
    scratch_end(scratch);

    //- karim: We can safely as  sume there is an existing mesh
    return existing_mesh;
}
