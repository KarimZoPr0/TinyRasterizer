//
// Created by Karim on 2023-10-07.
//


void D_Grid2D( game_color_buffer_t *buffer )
{
    for( U32 y = 0; y < buffer->height; y += 10 )
    {
        for( U32 x = 0; x < buffer->width; x += 10 )
        {
            if( x % 10 == 0 || y % 10 == 0 )
            {
                buffer->memory[ ( y * buffer->width ) + x ] = 0xFF333333;
            }
        }
    }
}

void D_Pixel2D( game_color_buffer_t *buffer, S32 x, S32 y, U32 color )
{
    if( x >= buffer->width || x < 0 || y >= buffer->height || y < 0 ) return;
    buffer->memory[ ( y * buffer->width ) + x ] = color;
}

void D_Triangle2D( game_color_buffer_t *buffer, S32 x0, S32 y0, S32 x1, S32 y1, S32 x2, S32 y2, U32 color )
{
    D_Line2D( buffer, x0, y0, x1, y1, color );
    D_Line2D( buffer, x1, y1, x2, y2, color );
    D_Line2D( buffer, x2, y2, x0, y0, color );
}

void D_Line2D( game_color_buffer_t *buffer, S32 x0, S32 y0, S32 x1, S32 y1, U32 color )
{
    S32 dx = x1 - x0;
    S32 dy = y1 - y0;

    U32 longest_side_length = abs( dx ) >= abs( dy ) ? abs( dx ) : abs( dy );

    F32 x_inc = (F32)dx / ( F32 ) longest_side_length;
    F32 y_inc = (F32)dy / ( F32 ) longest_side_length;

    F32 current_x = (F32)x0;
    F32 current_y = (F32)y0;

    for( U32 i = 0; i < longest_side_length; ++i )
    {
        D_Pixel2D( buffer, ( U32 ) roundf( current_x ), ( U32 ) roundf( current_y ), color );
        current_x += x_inc;
        current_y += y_inc;
    }
}

void D_Rect2D( game_color_buffer_t *buffer, S32 x, S32 y, U32 width, U32 height, U32 color )
{
    if( x >= buffer->width || x < 0 || y >= buffer->height || y < 0 ) return;
    for( U32 row = y; row < y + height; ++row )
    {
        for( U32 col = x; col < x + width; ++col )
        {
            D_Pixel2D( buffer, col, row, color );
        }
    }
}
