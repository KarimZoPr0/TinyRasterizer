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

int avg_depth_comparator(const void* a, const void* b)
{
    F32 diff = ((triangle_t*)b)->avg_depth - ((triangle_t*)a)->avg_depth;
    if (diff < 0) return -1;
    if (diff > 0) return 1;
    return 0;
}


#define SHOW_3D 1
root_function void
game_update_and_render(app_t* app)
{
    triangle_t* triangles_to_render = 0;
    U32 triangle_count = 0;
    vec3_t camera_position = {0, 0, 0};
    F32 fov_factor = 500;
    F32 speed = 0.01f;

    game_state_t* state = app->game_state;
    game_color_buffer_t* buffer = app->color_buffer;
    U32 keyboard[KEY_COUNT];
    memcpy(keyboard, app->input->keyboard, KEY_COUNT * sizeof(U32));

    /////////////////////////////
    //- karim: initialization
    //
    {
        if (!app->is_initialized)
        {
            state->mesh_table = push_array(&state->arena, mesh_table_t, 1);
            state->mesh_table->slot_count = 256;
            state->mesh_table->slots = push_array(&state->arena, mesh_slot_t, state->mesh_table->slot_count);

            state->nil_mesh = 0;
            {
                state->nil_mesh = push_array(&state->meshes_arena, mesh_t, 1);
                vertex_chunk_node_t* vertex_chunk = {0};
                {
                    vertex_chunk = push_array(&state->vertex_chunk_arena, vertex_chunk_node_t, 1);
                    vertex_chunk->cap = 256;
                    vertex_chunk->count = N_MESH_VERTICES;
                    vertex_chunk->v = push_array(&state->vertex_chunk_arena, vec3_t, vertex_chunk->cap);
                    memcpy(vertex_chunk->v, cube_vertices, sizeof(cube_vertices));
                    state->nil_mesh->vertex_chunks.chunk_count = 1;
                    state->nil_mesh->vertex_chunks.total_count = N_MESH_VERTICES;
                }
                face_chunk_node_t* face_chunk = {0};
                {
                    face_chunk = push_array(&state->face_chunk_arena, face_chunk_node_t, 1);
                    face_chunk->cap = 256;
                    face_chunk->count = N_MESH_FACES;
                    face_chunk->v = push_array(&state->face_chunk_arena, face_t, face_chunk->cap);
                    memcpy(face_chunk->v, cube_faces, sizeof(cube_faces));
                    state->nil_mesh->face_chunks.chunk_count = 1;
                    state->nil_mesh->face_chunks.total_count = N_MESH_FACES;
                }
                state->nil_mesh->vertex_chunks.first = vertex_chunk;
                state->nil_mesh->face_chunks.first = face_chunk;
            }

            state->nil_mesh->scale = (vec3_t){1.0f, 1.0f, 1.0f};

            app->is_initialized = true;
            app->render_mode = RENDER_WIRE;
            app->cull_mode = CULL_BACKFACE;
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
#if SHOW_3D
        state->mesh = mesh_from_key(state, "../assets/cube.obj");

        // state->mesh->translation.x += 0.01f;
        state->mesh->translation.z = 5.0f;
        // state->mesh->scale.x += 0.002f;
        state->mesh->rotation.x += 0.01f;
        state->mesh->rotation.y += 0.01f;
        state->mesh->rotation.z += 0.01f;

        //- karim: create a scale matrix that will be used to multiply the mesh vertices
        mat4_t translation_matrix = mat4_make_translation(state->mesh->translation.x, state->mesh->translation.y,
                                                          state->mesh->translation.z);
        mat4_t scale_matrix = mat4_make_scale(state->mesh->scale.x, state->mesh->scale.y, state->mesh->scale.z);
        mat4_t rotation_matrix_x = mat4_make_rotation_x(state->mesh->rotation.x);
        mat4_t rotation_matrix_y = mat4_make_rotation_y(state->mesh->rotation.y);
        mat4_t rotation_matrix_z = mat4_make_rotation_z(state->mesh->rotation.z);

        //- karim: create a world matrix combining scale, rotation and translation matrices
        mat4_t world_matrix = mat4_identity();
        world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
        world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
        world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
        world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
        world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

        const U32 num_faces = state->mesh->face_chunks.total_count;
        triangles_to_render = push_array(&state->frame_arena, triangle_t, num_faces);
        triangle_count = 0;

        //- karim: Loop over all face chunks of the mesh
        for (face_chunk_node_t* face_chunk = state->mesh->face_chunks.first; face_chunk != 0; face_chunk = face_chunk->
             next)
        {
            //- karim: Loop over all triangle faces of the mesh

            for (U64 i = 0; i < face_chunk->count; ++i)
            {
                face_t mesh_face = face_chunk->v[i];
                vec3_t face_vertices[3] = {
                    get_vertex_by_index(&state->mesh->vertex_chunks, mesh_face.a),
                    get_vertex_by_index(&state->mesh->vertex_chunks, mesh_face.b),
                    get_vertex_by_index(&state->mesh->vertex_chunks, mesh_face.c),
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
                if (app->cull_mode == CULL_BACKFACE)
                {
                    vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
                    vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
                    vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);

                    //- karim: get the vector subtraction of B-A and C-A
                    vec3_t vector_ab = vec3_sub(vector_b, vector_a);
                    vec3_t vector_ac = vec3_sub(vector_c, vector_a);

                    //- karim: compute the face normal (using cross product to find perpendicular)
                    vec3_t normal = vec3_cross(vector_ab, vector_ac);

                    //- karim: find the vector between a point in the triangle and the camera origin
                    vec3_t camera_ray = vec3_sub(camera_position, vector_a);

                    //- karim: calculate how aligned the camera ray is with the face normal (using dot product)
                    float dot_normal_camera = vec3_dot(camera_ray, normal);

                    //- karim: bypass the triangles that are looking away from the camera
                    if (dot_normal_camera < 0)
                    {
                        continue;
                    }
                }
                vec2_t projected_points[3];
                //- karim: loop all three vertices to perform projection
                for (U32 j = 0; j < 3; ++j)
                {
                    //- karim: project the current vertex
                    projected_points[j] = project(vec3_from_vec4(transformed_vertices[j]), fov_factor);

                    //- karim: translate the projected points to the middle of the screen
                    projected_points[j].x += (F32)buffer->width / 2;
                    projected_points[j].y += (F32)buffer->height / 2;
                }
                F32 avg_depth = transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z;
                triangle_t projected_triangle = {
                    .points = {
                        {projected_points[0].x, projected_points[0].y},
                        {projected_points[1].x, projected_points[1].y},
                        {projected_points[2].x, projected_points[2].y}
                    },
                    .color = mesh_face.color,
                    .avg_depth = avg_depth
                };
                //- karim: save the projected triangle in the array
                triangles_to_render[triangle_count++] = projected_triangle;
            }

            //- karim: sort the triangles to render by their avg_depth
            qsort(triangles_to_render, triangle_count, sizeof(triangle_t), avg_depth_comparator);
        }
#else
    //- karim: Existing 2D update code
    if (input->up == 1)
    {
        state->player.y -= state->offset;
    }
    else if (input->down == 1)
    {
        state->player.y += state->offset;
    }

    if (input->right)
    {
        state->player.x += state->offset;
    }
    else if (input->left)
    {
        state->player.x -= state->offset;
    }

    state->player.x = SDL_clamp(state->player.x, 0, buffer->width - state->player.w);
    state->player.y = SDL_clamp(state->player.y, 0, buffer->height - state->player.h);
#endif
    }

    //- karim: draw
    {
        draw_grid(buffer);

#if SHOW_3D

        const U32 num_triangles = triangle_count;
        for (U32 i = 0; i < num_triangles; i += 2)
        {
            triangle_t triangle = triangles_to_render[i];
            triangle_t second_triangle = triangles_to_render[i + 1];

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
                draw_filled_triangle(
                    buffer,
                    (S32)second_triangle.points[0].x, (S32)second_triangle.points[0].y,
                    (S32)second_triangle.points[1].x, (S32)second_triangle.points[1].y,
                    (S32)second_triangle.points[2].x, (S32)second_triangle.points[2].y,
                    second_triangle.color
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

                draw_triangle(
                    buffer,
                    (S32)second_triangle.points[0].x, (S32)second_triangle.points[0].y,
                    (S32)second_triangle.points[1].x, (S32)second_triangle.points[1].y,
                    (S32)second_triangle.points[2].x, (S32)second_triangle.points[2].y,
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
#else
            D_Rect2D(buffer, state->player.x, state->player.y, state->player.w + 10, state->player.h - 10, 0xFFFFAF5F);
#endif
    }
}
