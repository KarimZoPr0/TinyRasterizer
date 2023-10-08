#include "../include/array.h"
#include "../include/display.h"
#include "../include/vector.h"
#include "../include/mesh.h"

triangle_t *triangles_to_render = NULL;

vec3_t camera_position = { 0, 0, -100 };
vec3_t mesh_rotation = { 0, 0, 0 };

float fov_factor = 192;

bool is_running = false;
Uint32 previous_frame_time = 0;

void setup( )
{
    color_buffer = ( uint32_t * ) malloc( sizeof( uint32_t ) * window_width * window_height );

    color_buffer_texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            window_width,
            window_height
    );

    load_cube_mesh_data();
}

void process_input( )
{
    SDL_Event event;
    SDL_PollEvent( &event );

    switch( event.type )
    {
        case SDL_QUIT:
            is_running = false;
            break;
        case SDL_KEYDOWN:
            if( event.key.keysym.sym == SDLK_ESCAPE ) is_running = false;
            break;
    }
}

vec2_t project( vec3_t point )
{
    vec2_t projected_point = {
            ( fov_factor * point.x ), // / point.z,
            ( fov_factor * point.y ) // / point.z
    };
    return projected_point;
}

void update( )
{
    // Wait some time until the reach the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - ( SDL_GetTicks( ) - previous_frame_time );

    // Only delay execution if we are running too fast
    if( time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME )
    {
        SDL_Delay( time_to_wait );
    }

    triangles_to_render = NULL;

    previous_frame_time = SDL_GetTicks( );
    mesh.rotation.x = mesh.rotation.y = mesh.rotation.z += 0.01;

    // Loop all triangle faces of our mesh
    int num_faces = array_length(mesh.faces);
    for( int i = 0; i < num_faces; ++i )
    {
        face_t mesh_face = mesh.faces[ i ];
        vec3_t face_vertices[3] = {
                mesh.vertices[ mesh_face.a - 1 ],
                mesh.vertices[ mesh_face.b - 1 ],
                mesh.vertices[ mesh_face.c - 1 ]
        };

        triangle_t projected_triangle;

        // Loop all three vertices of this current face and apply transformations
        for( int j = 0; j < 3; ++j )
        {
            vec3_t transformed_vertex = face_vertices[ j ];

            // Rotate points
            transformed_vertex = vec3_rotate_x( transformed_vertex, mesh.rotation.x );
            transformed_vertex = vec3_rotate_y( transformed_vertex, mesh.rotation.y );
            transformed_vertex = vec3_rotate_z( transformed_vertex, mesh.rotation.z );

            // Translate the vertex away from the camera
            transformed_vertex.z -= camera_position.z;

            // Project the current vertex
            vec2_t projected_point = project( transformed_vertex );

            // Scale and translate the projected points to the middle of the screen
            projected_point.x += ( window_width / 2 );
            projected_point.y += ( window_height / 2 );

            // Save the projected 2D vector in the array of projected points
            projected_triangle.points[ j ] = projected_point;

        }
        // Save the projected triangle in the array of triangles to render
        array_push(triangles_to_render, projected_triangle);
    }

/*    for( int i = 0; i < N_POINTS; ++i )
    {
        vec3_t point = cube_points[ i ];

        // Rotate points
        vec3_t transformed_point = vec3_rotate_x( point, mesh_rotation.x );
        transformed_point = vec3_rotate_y( transformed_point, mesh_rotation.y );
        transformed_point = vec3_rotate_z( transformed_point, mesh_rotation.z );

        // Translate the points away from the camera
        transformed_point.z -= camera_position.z;

        // Project the current point
        vec2_t projected_point = project( transformed_point );

        // Save the projected 2D vector in the array of projected points
        projected_points[ i ] = projected_point;
    }*/
}


void render( )
{
    draw_grid( );

    // loop all projected triangles and render them

    int num_triangles = array_length(triangles_to_render);

    for( int i = 0; i < num_triangles; ++i )
    {
        triangle_t triangle = triangles_to_render[ i ];
        draw_rect( triangle.points[ 0 ].x, triangle.points[ 0 ].y, 3, 3, 0xFFFFFF00 );
        draw_rect( triangle.points[ 1 ].x, triangle.points[ 1 ].y, 3, 3, 0xFFFFFF00 );
        draw_rect( triangle.points[ 2 ].x, triangle.points[ 2 ].y, 3, 3, 0xFFFFFF00 );

        draw_triangle(
                triangle.points[ 0 ].x, triangle.points[ 0 ].y,
                triangle.points[ 1 ].x, triangle.points[ 1 ].y,
                triangle.points[ 2 ].x, triangle.points[ 2 ].y,
                0xFF00FF00
        );
    }

    // Clear the array of triangles to render every frame loop
    array_free(triangles_to_render);

    render_color_buffer( );
    clear_color_buffer( 0xFF000000 );

    SDL_RenderPresent( renderer );
}

void free_resources( )
{
    free( color_buffer );
    array_free(mesh.faces);
    array_free(mesh.vertices);
}

int main( int argc, char *argv[] )
{
    is_running = initialize_window( );

    setup( );

    while( is_running )
    {
        process_input( );
        update( );
        render( );
    }

    destroy_window( );
    free_resources();
    return 0;
}