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
    if( x > window_width || x < 0 || y > window_height || y < 0 ) return;
    color_buffer[ ( y * window_width ) + x ] = color;
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void draw_line( int x0, int y0, int x1, int y1, uint32_t color )
{
    int dx = x1 - x0;
    int dy = y1 - y0;

    int longest_side_length =  abs( dx ) >= abs( dy ) ? abs( dx ) : abs( dy );

    float x_inc = dx / ( float ) longest_side_length;
    float y_inc = dy / ( float ) longest_side_length;

    float current_x = x0;
    float current_y = y0;

    for( int i = 0; i < longest_side_length; ++i )
    {
        draw_pixel( roundf( current_x ), roundf( current_y ), color );
        current_x += x_inc;
        current_y += y_inc;
    }
}

void draw_rect( int x, int y, int width, int height, uint32_t color )
{
    if( x > window_width || x < 0 || y > window_height || y < 0 ) return;
    for( int row = y; row < y + height; ++row )
    {
        for( int col = x; col < x + width; ++col )
        {
            draw_pixel( col, row, color );
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
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit( );
}
