#include "../include/display.h"

bool is_running = false;

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

void update( )
{

}


void render( )
{
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );
    SDL_RenderClear( renderer );


    draw_grid( );
    draw_rect( 1400, 900, 200, 200, 0xFFFF0000 );
    draw_rect( 1150, 500, 200, 200, 0xFF00FF00 );
    draw_rect( 900, 900, 200, 200, 0xFF0000FF );

    draw_pixel(20,20, 0xFFFFFF00);

    render_color_buffer( );
    clear_color_buffer( 0xFF000000 );

    SDL_RenderPresent( renderer );
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
    return 0;
}