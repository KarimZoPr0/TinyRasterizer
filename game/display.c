//
// Created by Karim on 2023-10-07.
//

#include "../include/display.h"

#define SDL_MAIN_HANDLED
#include "SDL.h"

void D_Grid2D( game_color_buffer_t *buffer )
{
    for( int y = 0; y < buffer->height; y += 10 )
    {
        for( int x = 0; x < buffer->width; x += 10 )
        {
            if( x % 10 == 0 || y % 10 == 0 )
            {
                buffer->memory[ ( y * buffer->width ) + x ] = 0xFF333333;
            }
        }
    }
}

void D_Pixel2D( game_color_buffer_t *buffer, int x, int y, uint32_t color )
{
    if( x >= buffer->width || x < 0 || y >= buffer->height || y < 0 ) return;
    buffer->memory[ ( y * buffer->width ) + x ] = color;
}

void D_Triangle2D( game_color_buffer_t *buffer, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color )
{
    D_Line2D( buffer, x0, y0, x1, y1, color );
    D_Line2D( buffer, x1, y1, x2, y2, color );
    D_Line2D( buffer, x2, y2, x0, y0, color );
}

void D_Line2D( game_color_buffer_t *buffer, int x0, int y0, int x1, int y1, uint32_t color )
{
    int dx = x1 - x0;
    int dy = y1 - y0;

    int longest_side_length = abs( dx ) >= abs( dy ) ? abs( dx ) : abs( dy );

    float x_inc = (float)dx / ( float ) longest_side_length;
    float y_inc = (float)dy / ( float ) longest_side_length;

    float current_x = (float)x0;
    float current_y = (float)y0;

    for( int i = 0; i < longest_side_length; ++i )
    {
        D_Pixel2D( buffer, ( int ) roundf( current_x ), ( int ) roundf( current_y ), color );
        current_x += x_inc;
        current_y += y_inc;
    }
}

void D_Rect2D( game_color_buffer_t *buffer, int x, int y, int width, int height, uint32_t color )
{
    if( x >= buffer->width || x < 0 || y >= buffer->height || y < 0 ) return;
    for( int row = y; row < y + height; ++row )
    {
        for( int col = x; col < x + width; ++col )
        {
            D_Pixel2D( buffer, col, row, color );
        }
    }
}




