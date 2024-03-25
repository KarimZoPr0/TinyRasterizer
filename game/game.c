//
// Created by Karim on 3/22/2024.
//

#include "../include/array.h"
#include "../include/vector.h"
#include "game.h"
#include "../include/display.h"

#include "display.c"
#include "array.c"
#include "mesh.c"
#include "triangle.c"
#include "vector.c"

global vec3_t camera_position = { 0, 0, -100 };
global vec3_t mesh_rotation = { 0, 0, 0 };
global float fov_factor = 600;
global triangle_t *triangles_to_render = NULL;

vec2_t project( vec3_t point )
{
    vec2_t projected_point = {
            ( fov_factor * point.x ),
            ( fov_factor * point.y )
    };
    return projected_point;
}

function void
update( game_state_t *state, game_color_buffer_t *buffer )
{
    local_persist Uint32 previous_frame_time = 0;
    {
        U32 time_to_wait = FRAME_TARGET_TIME - ( SDL_GetTicks( ) - previous_frame_time );
        if( time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME )
        {
            SDL_Delay( time_to_wait );
        }
        previous_frame_time = SDL_GetTicks( );
    }

    triangles_to_render = NULL;
    state->mesh->rotation.x = state->mesh->rotation.y = state->mesh->rotation.z += 0.01f;

    // Loop all triangle faces of our mesh
    int num_faces = array_length( state->mesh->faces );
    for( int i = 0; i < num_faces; ++i )
    {
        face_t mesh_face = state->mesh->faces[ i ];
        vec3_t face_vertices[3] = {
                state->mesh->vertices[ mesh_face.a - 1 ],
                state->mesh->vertices[ mesh_face.b - 1 ],
                state->mesh->vertices[ mesh_face.c - 1 ]
        };

        triangle_t projected_triangle;
        {
            // Loop all three vertices of this current face and apply transformations
            for( int j = 0; j < 3; ++j )
            {
                vec3_t transformed_vertex = face_vertices[ j ];

                // Rotate points
                transformed_vertex = vec3_rotate_x( transformed_vertex, state->mesh->rotation.x );
                transformed_vertex = vec3_rotate_y( transformed_vertex, state->mesh->rotation.y );
                transformed_vertex = vec3_rotate_z( transformed_vertex, state->mesh->rotation.z );

                // Translate the vertex away from the camera
                transformed_vertex.z -= camera_position.z;

                // Project the current vertex
                vec2_t projected_point = project( transformed_vertex );

                // Scale and translate the projected points to the middle of the screen
                projected_point.x += ( (float)buffer->width / 2 );
                projected_point.y += ( (float)buffer->height / 2 );

                // Save the projected 2D vector in the array of projected points
                projected_triangle.points[ j ] = projected_point;

            }
        }

        // Save the projected triangle in the array of triangles to render
        array_push( triangles_to_render, projected_triangle );
    }


    return;
    if( state->player.x + 50 >= buffer->width || state->player.x < 0 )
    {
        state->offset *= -1;
    }

    state->player.x += state->offset * 0;
}

function void
render( game_state_t *state, game_color_buffer_t *buffer )
{
    D_Grid2D( buffer );

    int num_triangles = array_length( triangles_to_render );

    for( int i = 0; i < num_triangles; ++i )
    {
        triangle_t triangle = triangles_to_render[ i ];
        D_Rect2D( buffer, (int)triangle.points[ 1 ].x, (int)triangle.points[ 1 ].y, 3, 3, 0xFFFFFF00 );
        D_Rect2D( buffer, (int)triangle.points[ 0 ].x, (int)triangle.points[ 0 ].y, 3, 3, 0xFFFFFF00 );
        D_Rect2D( buffer, (int)triangle.points[ 2 ].x, (int)triangle.points[ 2 ].y, 3, 3, 0xFFFFFF00 );

        D_Triangle2D(
                buffer,
                ( int ) triangle.points[ 0 ].x, ( int ) triangle.points[ 0 ].y,
                ( int ) triangle.points[ 1 ].x, ( int ) triangle.points[ 1 ].y,
                ( int ) triangle.points[ 2 ].x, ( int ) triangle.points[ 2 ].y,
                0xFFFF00FF
        );
    }

    array_free( triangles_to_render );

    return;
    D_Rect2D( buffer, 1000, 700, 100, 100, 0xFF00FF00 );
    D_Rect2D( buffer, 500, 500, 100, 100, 0xFF00FFFF );
    D_Rect2D( buffer, state->player.x, state->player.y, state->player.w, state->player.h, 0xFFFF0FF0 );
}


root_function void
game_update_and_render( game_memory_t *game_memory, game_color_buffer_t *buffer )
{
    game_state_t *state = ( game_state_t * ) game_memory->memory;
    if( !game_memory->is_initialized )
    {
        state->player.x = state->player.y = 250;
        state->player.w = state->player.h = 100;
        state->offset = 10;
        state->mesh = (mesh_t *)((char *)game_memory->memory + sizeof(game_state_t));
        memset(state->mesh, 0, sizeof(*state->mesh));
        load_obj_file_data(state->mesh, "../assets/f22.obj");
        game_memory->is_initialized = true;
    }

    update( state, buffer );
    render( state, buffer );
}