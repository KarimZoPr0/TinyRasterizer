//
// Created by Karim on 3/22/2024.
//

// TODO: all headers should be in base_inc.h

#include "SDL.h"
#include "../include/base/base_inc.h"
#include "../include/mesh.h"
#include "../include/array.h"
#include "../include/game.h"
#include "../include/draw.h"
#include "../include/input.h"


//TODO: all sources should be in base_inc.c
#include "base/base_inc.c"
#include "draw.c"
#include "array.c"
#include "mesh.c"
#include "input.c"

#define SHOW_3D 1

global triangle_t* triangles_to_render = NULL;
global vec3_t camera_position = {0, 0, -4};
global F32 fov_factor = 600;

function void
update(game_state_t* state, game_input_t* input, game_color_buffer_t* buffer)
{
#if SHOW_3D
    triangles_to_render = NULL;
    state->mesh->rotation.x = state->mesh->rotation.y = state->mesh->rotation.z += 0.01f;

    // Loop all triangle faces of our mesh
    const U32 num_faces = array_length(state->mesh->faces);
    for (U32 i = 0; i < num_faces; ++i)
    {
        face_t mesh_face = state->mesh->faces[i];
        vec3_t face_vertices[3] = {
            state->mesh->vertices[mesh_face.a - 1],
            state->mesh->vertices[mesh_face.b - 1],
            state->mesh->vertices[mesh_face.c - 1]
        };

        triangle_t projected_triangle;
        {
            // Loop all three vertices of this current face and apply transformations
            for (U32 j = 0; j < 3; ++j)
            {
                vec3_t transformed_vertex = face_vertices[j];

                // Rotate points
                transformed_vertex = vec3_rotate_x(transformed_vertex, state->mesh->rotation.x);
                transformed_vertex = vec3_rotate_y(transformed_vertex, state->mesh->rotation.y);
                transformed_vertex = vec3_rotate_z(transformed_vertex, state->mesh->rotation.z);

                // Translate the vertex away from the camera
                transformed_vertex.z -= camera_position.z;

                // Scale the transformed vertex
                //TODO: Replace this vector operations
                transformed_vertex.x *= fov_factor;
                transformed_vertex.y *= fov_factor;

                // Project the current vertex
                vec2_t projected_point = project(transformed_vertex);

                // translate the projected points to the middle of the screen
                projected_point.x += (F32)buffer->width / 2;
                projected_point.y += (F32)buffer->height / 2;

                // Save the projected 2D vector in the array of projected points
                projected_triangle.points[j] = projected_point;
            }
        }

        // Save the projected triangle in the array of triangles to render
        array_push(triangles_to_render, projected_triangle);
    }
#else
    if(input->up == 1)
    {
        state->player.y -= state->offset;
    }
    else if(input->down == 1)
    {
        state->player.y += state->offset;
    }

    if( input->right )
    {
        state->player.x += state->offset;
    }
    else if( input->left )
    {
        state->player.x -= state->offset;
    }

    state->player.x = SDL_clamp(state->player.x, 0, buffer->width - state->player.w);
    state->player.y = SDL_clamp(state->player.y, 0, buffer->height - state->player.h);
#endif
}


function void
render(game_color_buffer_t* buffer)
{
    D_Grid2D(buffer);

#if SHOW_3D
    const U32 num_triangles = array_length(triangles_to_render);

    for (U32 i = 0; i < num_triangles; ++i)
    {
        triangle_t triangle = triangles_to_render[i];
        D_Rect2D(buffer, (S32)triangle.points[1].x, (S32)triangle.points[1].y, 3, 3, 0xFFFFFF00);
        D_Rect2D(buffer, (S32)triangle.points[0].x, (S32)triangle.points[0].y, 3, 3, 0xFFFFFF00);
        D_Rect2D(buffer, (S32)triangle.points[2].x, (S32)triangle.points[2].y, 3, 3, 0xFFFFFF00);

        D_Triangle2D(
            buffer,
            (S32)triangle.points[0].x, (S32)triangle.points[0].y,
            (S32)triangle.points[1].x, (S32)triangle.points[1].y,
            (S32)triangle.points[2].x, (S32)triangle.points[2].y,
            0x00AFFFFF
        );
    }

    array_free(triangles_to_render);

#else
    D_Rect2D( buffer, state->player.x, state->player.y, state->player.w, state->player.h, 0xFFFBBFAA );
#endif
}

root_function void
game_update_and_render(game_memory_t* game_memory, game_input_t* input, game_color_buffer_t* buffer)
{
    game_state_t* state = game_memory->memory;
    if (!game_memory->is_initialized)
    {
        state->player.x = state->player.y = 250;
        state->player.w = state->player.h = 100;
        state->offset = 10;
        state->mesh = (mesh_t*)game_memory->memory + sizeof(game_state_t);
        memset(state->mesh, 0, sizeof(*state->mesh));
        load_obj_file_data(state->mesh, "../assets/f22.obj");
        game_memory->is_initialized = true;
    }
    update(state, input, buffer);
    render(buffer);
}
