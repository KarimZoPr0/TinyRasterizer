//
// Created by Karim on 2023-10-08.
//

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
 {.a = 1, .b = 2, .c = 3, .color = 0xFFFF0000},
 {.a = 1, .b = 3, .c = 4, .color = 0xFFFF0000},
 // right
 {.a = 4, .b = 3, .c = 5, .color = 0xFF00FF00},
 {.a = 4, .b = 5, .c = 6, .color = 0xFF00FF00},
 // back
 {.a = 6, .b = 5, .c = 7, .color = 0xFF0000FF},
 {.a = 6, .b = 7, .c = 8, .color = 0xFF0000FF},
 // left
 {.a = 8, .b = 7, .c = 2, .color = 0xFFFFFF00},
 {.a = 8, .b = 2, .c = 1, .color = 0xFFFFFF00},
 // top
 {.a = 2, .b = 7, .c = 5, .color = 0xFFFF00FF},
 {.a = 2, .b = 5, .c = 3, .color = 0xFFFF00FF},
 // bottom
 {.a = 6, .b = 8, .c = 1, .color = 0xFFFFFFFF},
 {.a = 6, .b = 1, .c = 4, .color = 0xFFFFFFFF}
};

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
 for (vertex_chunk_node_t* n = vertex_chunks->first; n != 0; n = n->next)
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

function mesh_t* mesh_from_key(game_state_t* state, char* filename)
{
 arena_t* scratch_arena = scratch_begin();
 
 //- karim: return value
 mesh_t* existing_mesh = state->nil_entity.mesh;
 
 B32 file_changed = 0;
 
 //- karim: map key -> hash and slot
 U64 hash = hash_from_string(filename);
 U64 slot_idx = hash % state->mesh_table->slot_count;
 
 //- karim: find existing node in the table
 mesh_slot_t* slot = &state->mesh_table->slots[slot_idx];
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
 
 //- karim: has file changed
 FILETIME last_write_time = get_last_write_time(filename);
 file_changed = existing_node && CompareFileTime(&existing_node->v.last_write_time, &last_write_time) != 0;
 
 FILE* file = fopen(filename, "r");
 if (file_changed || (existing_mesh == state->nil_entity.mesh && file))
 {
  if (existing_node != 0)
  {
   existing_mesh = &existing_node->v;
   
   //- karim: free vertex chunks
   if (existing_mesh->vertex_chunks.first != 0)
   {
    existing_mesh->vertex_chunks.last->next = state->first_free_vertex_chunk;
    state->first_free_vertex_chunk = existing_mesh->vertex_chunks.first;
    existing_mesh->vertex_chunks.first = existing_mesh->vertex_chunks.last = 0;
    existing_mesh->vertex_chunks.total_count = existing_mesh->vertex_chunks.chunk_count = 0;
   }
   
   //- karim: free face chunks
   if (existing_mesh->face_chunks.first != 0)
   {
    existing_mesh->face_chunks.last->next = state->first_free_face_chunk;
    state->first_free_face_chunk = existing_mesh->face_chunks.first;
    existing_mesh->face_chunks.first = existing_mesh->face_chunks.last = 0;
    existing_mesh->face_chunks.total_count = existing_mesh->face_chunks.chunk_count = 0;
   }
  }
  else
  {
   //- karim: allocate a new mesh & node
   existing_node = push_array(&state->meshes_arena, mesh_node_t, 1);
   existing_mesh = &existing_node->v;
   existing_mesh->filename = filename;
   existing_mesh->last_write_time = last_write_time;
   SLLQueuePush(slot->first, slot->last, existing_node);
  }
  
  //- karim: parse the file
  char line[1024];
  while (fgets(line, sizeof(line), file))
  {
   if (strncmp(line, "v ", 2) == 0)
   {
    //- karim: vertex chunked linked-list
    vertex_chunk_node_t* n = existing_mesh->vertex_chunks.last;
    if (n == 0 || n->count >= n->cap)
    {
     n = state->first_free_vertex_chunk;
     if (n == 0)
     {
      n = push_array(&state->vertex_chunk_arena, vertex_chunk_node_t, 1);
      n->cap = 1024;
      n->v = push_array(&state->vertex_chunk_arena, vec3_t, n->cap);
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
    vec3_t* vertex = &n->v[n->count++];
    sscanf_s(line, "v %f %f %f", &vertex->x, &vertex->y, &vertex->z);
    existing_mesh->vertex_chunks.total_count++;
   }
   else if (strncmp(line, "f ", 2) == 0)
   {
    //- karim: face chunked linked-list
    face_chunk_node_t* n = existing_mesh->face_chunks.last;
    if (n == 0 || n->count >= n->cap)
    {
     n = state->first_free_face_chunk;
     if (n == 0)
     {
      n = push_array(&state->face_chunk_arena, face_chunk_node_t, 1);
      n->cap = 1024;
      n->v = push_array(&state->face_chunk_arena, face_t, n->cap);
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
    face_t* face = &n->v[n->count++];
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
  
  //- karim: update the last write time
  existing_mesh->last_write_time = last_write_time;
 }
 
 if (file )fclose(file);
 scratch_end(scratch_arena);
 
 //- karim: We can safely assume there is an existing mesh
 return existing_mesh;
}
