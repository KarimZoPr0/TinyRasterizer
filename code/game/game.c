//
// Created by Karim on 3/22/2024.
//

#include "../base/base_inc.h"
#include "../os/os_inc.h"
#include "../system/system_inc.h"
#include "../game/game.h"


#include "../base/base_inc.c"
#include "../os/os_inc.c"
#include "../system/system_inc.c"

#define STB_TRUETYPE_IMPLEMENTATION
#include "../external/stb_truetype.h"

#define SHOW_3D 1
root_function void
game_update_and_render(app_t* app)
{
    triangle_t* triangles_to_render = NULL;
    U32 triangle_count = 0;
    vec3_t camera_position = {0, 0, 0};
    F32 fov_factor = 800;
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
            state->player.x = state->player.y = 250;
            state->player.w = state->player.h = 100;
            state->offset = 10;

            // Initialize the mesh
            //state->mesh = push_array(&app->arena, mesh_t, 1);
            //load_cube_mesh_data(&app->mesh_arena, state->mesh);
            // load_obj_file_data(&app->arena, state->mesh, "../assets/cube.obj");
            state->mesh_table = push_array(&app->arena, mesh_table_t, 1);
            state->mesh_table->slot_count = 256;
            state->mesh_table->slots = push_array(&app->arena, mesh_slot_t, state->mesh_table->slot_count);

            state->nil_mesh = push_array(&app->arena, mesh_t, 1);
            init_nil_mesh(&app->arena, state->nil_mesh);

            app->is_initialized = true;
            app->render_mode = RENDER_WIRE;
            app->cull_mode = CULL_BACKFACE;
        }
    }

    /////////////////////////////
    //- karim: simulation
    //
    {
#if SHOW_3D

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

        nil_mesh = state->nil_mesh;
        state->mesh = mesh_from_key(&app->arena, state->mesh_table, "../assets/cube.obj");

        triangle_count = 0;

        state->rotation.x += speed;
        state->rotation.y += speed;
        state->rotation.z += speed;

        const U32 num_faces = state->mesh->face_chunks.total_count;
        triangles_to_render = push_array(&app->frame_arena, triangle_t, num_faces);

        // Loop over all face chunks of the mesh
        for (face_chunk_node_t* face_chunk = state->mesh->face_chunks.first; face_chunk; face_chunk = face_chunk->next)
        {
            // Loop over all triangle faces of the mesh
            for (U64 i = 0; i < face_chunk->count; ++i)
            {
                face_t mesh_face = face_chunk->v[i];
                vec3_t face_vertices[3] = {
                    get_vertex_by_index(&state->mesh->vertex_chunks, mesh_face.a),
                    get_vertex_by_index(&state->mesh->vertex_chunks, mesh_face.b),
                    get_vertex_by_index(&state->mesh->vertex_chunks, mesh_face.c),
                };

                triangle_t projected_triangle;

                vec3_t transformed_vertices[3];

                // Loop all three vertices of this current face and apply transformations
                for (U32 j = 0; j < 3; ++j)
                {
                    vec3_t transformed_vertex = face_vertices[j];

                    // Rotate points
                    transformed_vertex = vec3_rotate_x(transformed_vertex, state->rotation.x);
                    transformed_vertex = vec3_rotate_y(transformed_vertex, state->rotation.y);
                    transformed_vertex = vec3_rotate_z(transformed_vertex, state->rotation.z);

                    transformed_vertex.z += 5;

                    // Scale the transformed vertex
                    transformed_vertex.x *= fov_factor;
                    transformed_vertex.y *= fov_factor;

                    transformed_vertices[j] = transformed_vertex;
                }

                /////////////////////////////
                //- karim: backface culling
                //
                if (app->cull_mode == CULL_BACKFACE)
                {
                    vec3_t vector_a = transformed_vertices[0];
                    vec3_t vector_b = transformed_vertices[1];
                    vec3_t vector_c = transformed_vertices[2];

                    // Get the vector subtraction of B-A and C-A
                    vec3_t vector_ab = vec3_sub(vector_b, vector_a);
                    vec3_t vector_ac = vec3_sub(vector_c, vector_a);

                    // Compute the face normal (using cross product to find perpendicular)
                    vec3_t normal = vec3_cross(vector_ab, vector_ac);

                    // Find the vector between a point in the triangle and the camera origin
                    vec3_t camera_ray = vec3_sub(camera_position, vector_a);

                    // Calculate how aligned the camera ray is with the face normal (using dot product)
                    float dot_normal_camera = vec3_dot(camera_ray, normal);

                    // Bypass the triangles that are looking away from the camera
                    if (dot_normal_camera < 0)
                    {
                        continue;
                    }
                }

                // Loop all three vertices to perform projection
                for (U32 j = 0; j < 3; ++j)
                {
                    // Project the current vertex
                    vec2_t projected_point = project(transformed_vertices[j]);

                    // Translate the projected points to the middle of the screen
                    projected_point.x += (F32)buffer->width / 2;
                    projected_point.y += (F32)buffer->height / 2;

                    // Save the projected 2D vector in the triangle
                    projected_triangle.points[j] = projected_point;
                }

                // Save the projected triangle in the array
                triangles_to_render[triangle_count++] = projected_triangle;
            }
        }
#else
    // Existing 2D update code
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

    /////////////////////////////
    //- karim: draw
    //
    {
        draw_grid(buffer);

#if SHOW_3D

        const U32 num_triangles = triangle_count;
        for (U32 i = 0; i < num_triangles; ++i)
        {
            triangle_t triangle = triangles_to_render[i];

            if (app->render_mode == RENDER_FILL_TRIANGLE || app->render_mode == RENDER_FILL_TRIANGLE_WIRE)
            {
                // Draw filled triangle
                draw_filled_triangle(
                    buffer,
                    (S32)triangle.points[0].x, (S32)triangle.points[0].y,
                    (S32)triangle.points[1].x, (S32)triangle.points[1].y,
                    (S32)triangle.points[2].x, (S32)triangle.points[2].y,
                    0xFF354E46
                );
            }

            // Draw triangle wireframe
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
#else
            D_Rect2D(buffer, state->player.x, state->player.y, state->player.w + 10, state->player.h - 10, 0xFFFFAF5F);
#endif
    }
}
