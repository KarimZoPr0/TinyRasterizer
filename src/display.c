//
// Created by Karim on 2023-10-07.
//

#include "../include/display.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
uint32_t *color_buffer = NULL;
SDL_Texture *color_buffer_texture = NULL;

int window_width = 800;
int window_height = 800;

bool initialize_window( void )
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
    {
        fprintf( stderr, "Error initializing SDL \n" );
        return false;
    }

    // Use SDL to query what is the fullscreen max. width anda height
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode( 0, &display_mode );
    window_width = display_mode.w;
    window_height = display_mode.h;

    window = SDL_CreateWindow(
            NULL,
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            window_width,
            window_height,
            SDL_WINDOW_FULLSCREEN_DESKTOP
    );

    if( !window )
    {
        fprintf( stderr, "Error initializing SDL window. \n" );
        return false;
    }

    renderer = SDL_CreateRenderer( window, -1, 0 );

    if( !renderer )
    {
        fprintf( stderr, "Error initializing SDL renderer. \n" );
        return false;
    }

    SDL_SetWindowFullscreen( window, SDL_WINDOW_FULLSCREEN_DESKTOP );
    return true;
}

void render_color_buffer( )
{
    SDL_UpdateTexture(
            color_buffer_texture,
            NULL,
            color_buffer,
            ( int ) ( window_width * sizeof( uint32_t ) )
    );

    SDL_RenderCopy( renderer, color_buffer_texture, NULL, NULL );
}


void draw_grid( )
{
    for( int y = 0; y < window_height; y += 10 )
    {
        for( int x = 0; x < window_width; x += 10 )
        {
            if( x % 10 == 0 || y % 10 == 0 )
            {
                color_buffer[ ( y * window_width ) + x ] = 0xFF333333;
            }
        }
    }
}

void draw_pixel( int x, int y, uint32_t color )
{
    if(x > window_width || x < 0 || y > window_height || y < 0) return;
    color_buffer[ ( y * window_width ) + x ] = color;
}

void draw_rect( int x, int y, int width, int height, uint32_t color )
{
    if(x > window_width || x < 0 || y > window_height || y < 0) return;
    for( int row = y; row < y + height; ++row )
    {
        for( int col = x; col < x + width; ++col )
        {
            draw_pixel(col, row, color);
        }
    }
}

void clear_color_buffer( uint32_t color )
{
    for( int y = 0; y < window_height; ++y )
    {
        for( int x = 0; x < window_width; ++x )
        {
            color_buffer[ ( y * window_width ) + x ] = color;
        }
    }
}


void destroy_window( )
{
    free( color_buffer );
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit( );
}
