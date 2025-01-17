//
// Created by Karim on 3/22/2024.
//

#include "../external/spall.h"

#include "../base/base_inc.h"
#include "../os/os_inc.h"
#include "../system/system_inc.h"
#include "../game/game.h"

#include "../base/base_inc.c"
#include "../os/os_inc.c"
#include "../system/system_inc.c"

double get_time_in_micros(void)
{
 static double invfreq;
 if (!invfreq)
 {
  LARGE_INTEGER frequency;
  QueryPerformanceFrequency(&frequency);
  invfreq = 1000000.0 / frequency.QuadPart;
 }
 LARGE_INTEGER counter;
 QueryPerformanceCounter(&counter);
 return counter.QuadPart * invfreq;
}

int avg_depth_comparator(const void* a, const void* b)
{
 F32 diff = ((triangle_t*)b)->avg_depth - ((triangle_t*)a)->avg_depth;
 if (diff < 0) return -1;
 if (diff > 0) return 1;
 return 0;
}

root_function void
game_update_and_render(app_t* app)
{
 triangle_t* triangles_to_render = 0;
 U32 triangle_count = 0;
 vec3_t camera_position = {0, 0, 0};
 F32 fov = 200;
 F32 aspect = (F32)app->color_buffer->width / (F32)app->color_buffer->height;
 F32 z_near = 0.1f;
 F32 z_far = 100.0f;
 mat4_t proj_matrix = mat4_make_perspective(fov, aspect, z_near, z_far);
 
 light_t light = {0, 0, 1};
 
 game_state_t* state = app->game_state;
 game_color_buffer_t* buffer = app->color_buffer;
 U32 keyboard[KEY_COUNT];
 memcpy(keyboard, app->input->keyboard, KEY_COUNT * sizeof(U32));
 
 /////////////////////////////
 //- karim: initialization
 //
 {
  if (app->is_initialized == 0)
  {
   state->mesh_table = push_array(&state->arena, mesh_table_t, 1);
   state->mesh_table->slot_count = 256;
   state->mesh_table->slots = push_array(&state->arena, mesh_slot_t, state->mesh_table->slot_count);
   
   state->nil_entity.mesh = 0;
   {
    state->nil_entity.mesh = push_array(&state->meshes_arena, mesh_t, 1);
    vertex_chunk_node_t* vertex_chunk = {0};
    {
     vertex_chunk = push_array(&state->vertex_chunk_arena, vertex_chunk_node_t, 1);
     vertex_chunk->cap = 256;
     vertex_chunk->count = N_MESH_VERTICES;
     vertex_chunk->v = push_array(&state->vertex_chunk_arena, vec3_t, vertex_chunk->cap);
     memcpy(vertex_chunk->v, cube_vertices, sizeof(cube_vertices));
     state->nil_entity.mesh->vertex_chunks.chunk_count = 1;
     state->nil_entity.mesh->vertex_chunks.total_count = N_MESH_VERTICES;
    }
    face_chunk_node_t* face_chunk = {0};
    {
     face_chunk = push_array(&state->face_chunk_arena, face_chunk_node_t, 1);
     face_chunk->cap = 256;
     face_chunk->count = N_MESH_FACES;
     face_chunk->v = push_array(&state->face_chunk_arena, face_t, face_chunk->cap);
     memcpy(face_chunk->v, cube_faces, sizeof(cube_faces));
     state->nil_entity.mesh->face_chunks.chunk_count = 1;
     state->nil_entity.mesh->face_chunks.total_count = N_MESH_FACES;
    }
    state->nil_entity.mesh->vertex_chunks.first = vertex_chunk;
    state->nil_entity.mesh->face_chunks.first = face_chunk;
   }
   
   state->nil_entity.scale = (vec3_t){1.0f, 1.0f, 1.0f};
   state->e1.scale = (vec3_t){1.0f, 1.0f, 1.0f};
   
   app->render_mode = RENDER_WIRE;
   app->cull_mode = CULL_BACKFACE;
   
   app->is_initialized = 1;
  }
 }
 
 //- karim: input -> render mode
 {
  if (keyboard[KEY_1])
  {
   app->render_mode = RENDER_WIRE_VERTEX;
  }
  else if (keyboard[KEY_2])
  {
   app->render_mode = RENDER_WIRE;
  }
  else if (keyboard[KEY_3])
  {
   app->render_mode = RENDER_FILL_TRIANGLE;
  }
  else if (keyboard[KEY_4])
  {
   app->render_mode = RENDER_FILL_TRIANGLE_WIRE;
  }
  else if (keyboard[KEY_C])
  {
   app->cull_mode = CULL_BACKFACE;
  }
  else if (keyboard[KEY_D])
  {
   app->cull_mode = CULL_NONE;
  }
 }
 
 //- karim: update
 {
  state->e1.mesh = mesh_from_key(state, "../assets/cube.obj");
  
  // state->e1.translation.x += 0.01f;
  state->e1.translation.z = 5.0f;
  // state->e1.scale.x += 0.002f;
  state->e1.rotation.x += 0.01f;
  state->e1.rotation.y += 0.01f;
  state->e1.rotation.z += 0.01f;
  
  //- karim: create a scale matrix that will be used to multiply the mesh vertices7
  mat4_t translation_matrix = mat4_make_translation(state->e1.translation.x, state->e1.translation.y,
                                                    state->e1.translation.z);
  mat4_t scale_matrix = mat4_make_scale(state->e1.scale.x, state->e1.scale.y, state->e1.scale.z);
  mat4_t rotation_matrix_x = mat4_make_rotation_x(state->e1.rotation.x);
  mat4_t rotation_matrix_y = mat4_make_rotation_y(state->e1.rotation.y);
  mat4_t rotation_matrix_z = mat4_make_rotation_z(state->e1.rotation.z);
  
  //- karim: create a world matrix combining scale, rotation and translation matrices
  mat4_t world_matrix = mat4_identity();
  world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
  world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
  world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
  world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
  world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);
  
  U32 num_faces = state->e1.mesh->face_chunks.total_count;
  triangles_to_render = push_array(&state->frame_arena, triangle_t, num_faces);
  
  //- karim: Loop over all face chunks of the mesh
  for (face_chunk_node_t* face_chunk = state->e1.mesh->face_chunks.first; face_chunk != 0; face_chunk = face_chunk->next)
  {
   //- karim: Loop over all triangle faces of the mesh
   for (U64 i = 0; i < face_chunk->count; ++i)
   {
    face_t mesh_face = face_chunk->v[i];
    vec3_t face_vertices[3] = {
     get_vertex_by_index(&state->e1.mesh->vertex_chunks, mesh_face.a),
     get_vertex_by_index(&state->e1.mesh->vertex_chunks, mesh_face.b),
     get_vertex_by_index(&state->e1.mesh->vertex_chunks, mesh_face.c),
    };
    
    vec4_t transformed_vertices[3];
    //- karim: loop all three vertices of this current face and apply transformations
    for (U32 j = 0; j < 3; ++j)
    {
     vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);
     
     //- karim: multiply the world matrix by the original vector
     transformed_vertex = mat4_mul_vec4(world_matrix, transformed_vertex);
     
     transformed_vertices[j] = transformed_vertex;
    }
    
    //- karim: backface culling
    
    vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
    vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
    vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);
    
    //- karim: get the vector subtraction of B-A and C-A
    vec3_t vector_ab = vec3_normalize(vec3_sub(vector_b, vector_a));
    vec3_t vector_ac = vec3_normalize(vec3_sub(vector_c, vector_a));
    
    //- karim: compute the face normal (using cross product to find perpendicular)
    vec3_t normal = vec3_normalize(vec3_cross(vector_ab, vector_ac));
    
    //- karim: find the vector between a point in the triangle and the camera origin
    vec3_t camera_ray = vec3_sub(camera_position, vector_a);
    
    //- karim: calculate how aligned the camera ray is with the face normal (using dot product)
    F32 dot_normal_camera = vec3_dot(camera_ray, normal);
    
    if (app->cull_mode == CULL_BACKFACE)
    {
     //- karim: bypass the triangles that are looking away from the camera
     if (dot_normal_camera < 0)
     {
      continue;
     }
    }
    
    
    vec4_t projected_points[3];
    //- karim: loop all three vertices to perform projection
    for (U32 j = 0; j < 3; ++j)
    {
     //- karim: project the current vertex
     projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);
     
     //- karim: scale into the view
     projected_points[j].x *= (F32)buffer->width / 2.0f;
     projected_points[j].y *= (F32)buffer->height / 2.0f;
     
     //- karim: invert the y values to account for flipped screen y coordinate
     projected_points[j].y *= -1;
     
     //- karim: translate the projected points to the middle of the screen
     projected_points[j].x += (F32)buffer->width / 2.0f;
     projected_points[j].y += (F32)buffer->height / 2.0f;
    }
    
    //- karim: calculate average depth
    F32 avg_depth = transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z;
    
    //- karim: calculate the shade intensity based on how aligned is the face normal and the light
    F32 light_intensity_factor = -vec3_dot(normal, light.direction);
    
    //- karim: calculate triangle color based on the light angle
    U32 triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);
    
    triangle_t projected_triangle = {
     .points = {
      {projected_points[0].x, projected_points[0].y},
      {projected_points[1].x, projected_points[1].y},
      {projected_points[2].x, projected_points[2].y}
     },
     .color = triangle_color,
     .avg_depth = avg_depth
    };
    
    
    //- karim: save the projected triangle in the array
    triangles_to_render[triangle_count++] = projected_triangle;
   }
   
   //- karim: sort the triangles to render by their avg_depth
   qsort(triangles_to_render, triangle_count, sizeof(triangle_t), avg_depth_comparator);
  }
 }
 
 //- karim: draw
 {
  draw_grid(buffer);
  const U32 num_triangles = triangle_count;
  for (U32 i = 0; i < num_triangles; i += 1)
  {
   triangle_t triangle = triangles_to_render[i];
   
   if (app->render_mode == RENDER_FILL_TRIANGLE || app->render_mode == RENDER_FILL_TRIANGLE_WIRE)
   {
    //- karim: draw filled triangle
    draw_filled_triangle(
                         buffer,
                         (S32)triangle.points[0].x, (S32)triangle.points[0].y,
                         (S32)triangle.points[1].x, (S32)triangle.points[1].y,
                         (S32)triangle.points[2].x, (S32)triangle.points[2].y,
                         triangle.color
                         );
   }
   
   //- karim: draw triangle wireframe
   if (app->render_mode == RENDER_WIRE || app->render_mode == RENDER_WIRE_VERTEX || app->render_mode ==
       RENDER_FILL_TRIANGLE_WIRE)
   {
    draw_triangle(
                  buffer,
                  (S32)triangle.points[0].x, (S32)triangle.points[0].y,
                  (S32)triangle.points[1].x, (S32)triangle.points[1].y,
                  (S32)triangle.points[2].x, (S32)triangle.points[2].y,
                  ARGB(255, 255, 255, 255)
                  );
   }
   
   if (app->render_mode == RENDER_WIRE_VERTEX)
   {
    draw_rect(buffer, (S32)triangle.points[0].x - 3, (S32)triangle.points[0].y - 3, 6, 6,
              ARGB(255, 255, 0, 0));
    draw_rect(buffer, (S32)triangle.points[1].x - 3, (S32)triangle.points[1].y - 3, 6, 6,
              ARGB(255, 255, 0, 0));
    draw_rect(buffer, (S32)triangle.points[2].x - 3, (S32)triangle.points[2].y - 3, 6, 6,
              ARGB(255, 255, 0, 0));
   }
  }
 }
}
