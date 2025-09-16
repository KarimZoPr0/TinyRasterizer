//
// Created by Karim on 3/22/2024.
//

#include "SDL.h"
#include "../external/spall.h"
#include "../base/base_inc.h"
#include "../os/os_inc.h"
#include "../system/system_inc.h"
#include "../game/game.h"

#include "../base/base_inc.c"
#include "../os/os_inc.c"
#include "../system/system_inc.c"

global camera_t camera = {
    .position = {0, 0, 0},
    .direction = {0, 0, 1},
    .forward_velocity = {0, 0, 0},
    .yaw = 0.0
};

function int avg_depth_comparator(const void *a, const void *b)
{
    F32 diff = ((triangle_t*)b)->avg_depth - ((triangle_t*)a)->avg_depth;
    if (diff < 0) return -1;
    if (diff > 0) return 1;
    return 0;
}

static U32 profiler_initialized = 0;

root_function void
game_update_and_render(app_t *app)
{
    if (profiler_initialized == 0)
    {
        ProfileInit("Game");
        ProfileInitThread();
        profiler_initialized = 1;
    }

    // Initialization block
    if (app->is_initialized == 0)
    {
        profile_scope_begin("initialize");
        {
            arena_t *arena = arena_create();
            arena_t *meshes_arena = arena_create();
            arena_t *vertex_chunk_arena = arena_create();
            arena_t *face_chunk_arena = arena_create();

            app->game_state = push_array(arena, game_state_t, 1);
            app->game_state->arena = arena;
            app->game_state->meshes_arena = meshes_arena;
            app->game_state->vertex_chunk_arena = vertex_chunk_arena;
            app->game_state->face_chunk_arena = face_chunk_arena;
        }
        app->game_state->mesh_table = push_array(app->game_state->arena, mesh_table_t, 1);
        app->game_state->mesh_table->slot_count = 256;
        app->game_state->mesh_table->slots = push_array(app->game_state->arena, mesh_slot_t,
                                                        app->game_state->mesh_table->slot_count);

        app->game_state->nil_entity.mesh = 0;
        {
            app->game_state->nil_entity.mesh = push_array(app->game_state->meshes_arena, mesh_t, 1);
            vertex_chunk_node_t *vertex_chunk = {0};
            {
                vertex_chunk = push_array_no_zero(app->game_state->vertex_chunk_arena, vertex_chunk_node_t, 1);
                vertex_chunk->cap = 256;
                vertex_chunk->count = N_CUBE_VERTICES;
                vertex_chunk->v = push_array(app->game_state->vertex_chunk_arena, vec3_t, vertex_chunk->cap);
                memcpy(vertex_chunk->v, cube_vertices, sizeof(cube_vertices));
                app->game_state->nil_entity.mesh->vertex_chunks.chunk_count = 1;
                app->game_state->nil_entity.mesh->vertex_chunks.total_count = N_CUBE_VERTICES;
            }
            face_chunk_node_t *face_chunk = {0};
            {
                face_chunk = push_array_no_zero(app->game_state->face_chunk_arena, face_chunk_node_t, 1);
                face_chunk->cap = 256;
                face_chunk->count = N_CUBE_FACES;
                face_chunk->v = push_array(app->game_state->face_chunk_arena, face_t, face_chunk->cap);
                memcpy(face_chunk->v, cube_faces, sizeof(cube_faces));
                app->game_state->nil_entity.mesh->face_chunks.chunk_count = 1;
                app->game_state->nil_entity.mesh->face_chunks.total_count = N_CUBE_FACES;
            }
            app->game_state->nil_entity.mesh->vertex_chunks.first = vertex_chunk;
            app->game_state->nil_entity.mesh->face_chunks.first = face_chunk;
        }

        mesh_texture = (U32*)REDBRICK_TEXTURE;
        texture_width = 64;
        texture_height = 64;

        app->game_state->nil_entity.scale = (vec3_t){1.0f, 1.0f, 1.0f};
        app->game_state->e1.scale = (vec3_t){1.0f, 1.0f, 1.0f};

        app->render_flags = RENDER_FLAG_WIRE;
        app->cull_flags = CULL_FLAG_BACKFACE;

        app->is_initialized = 1;
        profile_scope_end();
    }

    triangle_t *triangles_to_render = 0;
    U32 triangle_count = 0;
    F32 fov = 200;
    F32 z_near = 0.1f;
    F32 z_far = 100.0f;
    light_t light = {0, 0, 1};
    F32 aspect = (F32)app->color_buffer->width / (F32)app->color_buffer->height;
    mat4_t proj_matrix = mat4_make_perspective(fov, aspect, z_near, z_far);
    game_state_t *state = app->game_state;
    game_color_buffer_t *buffer = app->color_buffer;
    U32 *keyboard = app->input->keyboard;
    F32 delta_time = app->delta_time;


    // Input handling block
    {
        if (keyboard[KEY_1])
        {
            keyboard[KEY_1] = 0;
            app->render_flags ^= RENDER_FLAG_VERTEX;
        }
        if (keyboard[KEY_2])
        {
            keyboard[KEY_2] = 0;
            app->render_flags ^= RENDER_FLAG_WIRE;
        }
        if (keyboard[KEY_3])
        {
            keyboard[KEY_3] = 0;
            app->render_flags ^= RENDER_FLAG_FILL;
        }
        if (keyboard[KEY_C])
        {
            keyboard[KEY_C] = 0;
            app->cull_flags ^= CULL_FLAG_BACKFACE;
        }

        // Camera input
        if (keyboard[KEY_Q])
        {
            camera.position.y += 3.0f * delta_time;
        }
        if (keyboard[KEY_E])
        {
            camera.position.y -= 3.0f * delta_time;
        }
        if (keyboard[KEY_A])
        {
            camera.yaw += 1.0f * delta_time;
        }
        if (keyboard[KEY_D])
        {
            camera.yaw -= 1.0f * delta_time;
        }
        if (keyboard[KEY_W])
        {
            camera.forward_velocity = vec3_mul(camera.direction, 5.0f * delta_time);
            camera.position = vec3_add(camera.position, camera.forward_velocity);
        }
        if (keyboard[KEY_S])
        {
            camera.forward_velocity = vec3_mul(camera.direction, 5.0f * delta_time);
            camera.position = vec3_sub(camera.position, camera.forward_velocity);
        }
    }

    profile_scope_begin("update");
    {
        // Entity update
        state->e1.mesh = mesh_from_key_new(state, "../assets/cube.obj");
        state->e1.translation.z = 5.0f;
        state->e1.rotation.x += 1.0f * delta_time;
        // state->e1.rotation.y += 1.0f * delta_time;
        // state->e1.rotation.z += 1.0f * delta_time;

        // Create the view matrix
        vec3_t up_direction = {0, 1, 0};
        vec3_t target = {0, 0, 1};
        mat4_t camera_yaw_rotation = mat4_make_rotation_y(camera.yaw);
        camera.direction = vec3_from_vec4(mat4_mul_vec4(camera_yaw_rotation, vec4_from_vec3(target)));
        target = vec3_add(camera.position, camera.direction);
        mat4_t view_matrix = mat4_look_at(camera.position, target, up_direction);

        // Create transformation matrices
        mat4_t translation_matrix = mat4_make_translation(state->e1.translation.x, state->e1.translation.y,
                                                          state->e1.translation.z);
        mat4_t scale_matrix = mat4_make_scale(state->e1.scale.x, state->e1.scale.y, state->e1.scale.z);
        mat4_t rotation_matrix_x = mat4_make_rotation_x(state->e1.rotation.x);
        mat4_t rotation_matrix_y = mat4_make_rotation_y(state->e1.rotation.y);
        mat4_t rotation_matrix_z = mat4_make_rotation_z(state->e1.rotation.z);

        // Combine scale, rotation and translation to form world matrix
        mat4_t world_matrix = mat4_identity();
        world_matrix = mat4_mul_mat4(scale_matrix, world_matrix);
        world_matrix = mat4_mul_mat4(rotation_matrix_x, world_matrix);
        world_matrix = mat4_mul_mat4(rotation_matrix_y, world_matrix);
        world_matrix = mat4_mul_mat4(rotation_matrix_z, world_matrix);
        world_matrix = mat4_mul_mat4(translation_matrix, world_matrix);

        // Model-View matrix
        mat4_t model_view = mat4_mul_mat4(view_matrix, world_matrix);

        triangles_to_render = push_array(app->frame_arena, triangle_t, state->e1.mesh->face_array.count);

        // Loop over all triangle faces of the mesh
        for (U64 i = 0; i < state->e1.mesh->face_array.count; ++i)
        {
            face_t mesh_face = state->e1.mesh->face_array.v[i];
            vec3_t face_vertices[3] = {
                state->e1.mesh->vertex_array.v[mesh_face.a - 1],
                state->e1.mesh->vertex_array.v[mesh_face.b - 1],
                state->e1.mesh->vertex_array.v[mesh_face.c - 1],
            };

            vec4_t transformed_vertices[3];
            // Vertex transformation profiling
            profile_scope_begin("vertex transformation");
            for (U32 j = 0; j < 3; ++j)
            {
                transformed_vertices[j] = mat4_mul_vec4(model_view, vec4_from_vec3(face_vertices[j]));
            }
            profile_scope_end();

            // Backface culling profiling
            profile_scope_begin("backface culling");
            vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]);
            vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]);
            vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]);

            vec3_t vector_ab = vec3_normalize(vec3_sub(vector_b, vector_a));
            vec3_t vector_ac = vec3_normalize(vec3_sub(vector_c, vector_a));

            vec3_t normal = vec3_normalize(vec3_cross(vector_ab, vector_ac));
            vec3_t origin = {0, 0, 0};
            vec3_t camera_ray = vec3_sub(origin, vector_a);
            F32 dot_normal_camera = vec3_dot(camera_ray, normal);
            profile_scope_end();

            if (app->cull_flags & CULL_FLAG_BACKFACE)
            {
                if (dot_normal_camera < 0)
                {
                    continue;
                }
            }

            vec4_t projected_points[3];
            // Projection profiling
            profile_scope_begin("projection");
            for (U32 j = 0; j < 3; ++j)
            {
                projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);
                projected_points[j].x *= (F32)buffer->width * 0.5f;
                projected_points[j].y *= (F32)buffer->height * 0.5f;
                projected_points[j].y *= -1;
                projected_points[j].x += (F32)buffer->width * 0.5f;
                projected_points[j].y += (F32)buffer->height * 0.5f;
            }
            profile_scope_end();

            // Calculate average depth
            F32 avg_depth = transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z;
            // Lighting calculation
            F32 light_intensity_factor = -vec3_dot(normal, light.direction);
            U32 triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);

            triangle_t projected_triangle = {
                .points = {
                    {projected_points[0].x, projected_points[0].y},
                    {projected_points[1].x, projected_points[1].y},
                    {projected_points[2].x, projected_points[2].y}
                },
                .texcoords = {
                    {mesh_face.a_uv.u, mesh_face.a_uv.v},
                    {mesh_face.b_uv.u, mesh_face.b_uv.v},
                    {mesh_face.c_uv.u, mesh_face.c_uv.v},
                },
                .color = triangle_color,
                .avg_depth = avg_depth
            };

            // Save the projected triangle in the array
            triangles_to_render[triangle_count++] = projected_triangle;
        }
        // Sort triangles profiling
        profile_scope_begin("sort triangles");
        qsort(triangles_to_render, triangle_count, sizeof(triangle_t), avg_depth_comparator);
        profile_scope_end();
    }
    profile_scope_end();

    profile_scope_begin("draw");
    {
        draw_grid(buffer);
        const U32 num_triangles = triangle_count;
        for (U32 i = 0; i < num_triangles; i += 1)
        {
            triangle_t triangle = triangles_to_render[i];

            // Draw triangle vertices
            if (app->render_flags & RENDER_FLAG_VERTEX)
            {
                draw_rect(buffer, (S32)triangle.points[0].x - 3, (S32)triangle.points[0].y - 3, 6, 6,
                          ARGB(255, 255, 0, 0));
                draw_rect(buffer, (S32)triangle.points[1].x - 3, (S32)triangle.points[1].y - 3, 6, 6,
                          ARGB(255, 255, 0, 0));
                draw_rect(buffer, (S32)triangle.points[2].x - 3, (S32)triangle.points[2].y - 3, 6, 6,
                          ARGB(255, 255, 0, 0));
            }

            // Draw triangle wireframe
            if (app->render_flags & RENDER_FLAG_WIRE)
            {
                draw_triangle(
                    buffer,
                    (S32)triangle.points[0].x, (S32)triangle.points[0].y,
                    (S32)triangle.points[1].x, (S32)triangle.points[1].y,
                    (S32)triangle.points[2].x, (S32)triangle.points[2].y,
                    ARGB(255, 255, 255, 255)
                );
            }

            // Draw textured triangle
            if (app->render_flags & RENDER_FLAG_TEXTURE)
            {
            }

            // Draw filled triangle
            if (app->render_flags & RENDER_FLAG_FILL)
            {
                draw_filled_triangle(
                    buffer,
                    (S32)triangle.points[0].x, (S32)triangle.points[0].y,
                    (S32)triangle.points[1].x, (S32)triangle.points[1].y,
                    (S32)triangle.points[2].x, (S32)triangle.points[2].y,
                    triangle.color
                );
            }
        }
    }
    profile_scope_end();

    ProfileQuitThread();
}
