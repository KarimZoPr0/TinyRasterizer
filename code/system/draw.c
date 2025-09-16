//
// Created by Karim on 2023-10-07.
//


function void draw_grid(game_color_buffer_t* buffer)
{
    for (S32 y = 0; y < buffer->height; y += 10)
    {
        for (S32 x = 0; x < buffer->width; x += 10)
        {
            buffer->memory[(y * buffer->width) + x] = 0xFF333333;
        }
    }
}

function void draw_pixel(game_color_buffer_t* buffer, S32 x, S32 y, U32 color)
{
    if (x < 0 || x >= buffer->width || y < 0 || y >= buffer->height) return;
    buffer->memory[(y * buffer->width) + x] = color;
}

function void draw_line(game_color_buffer_t* buffer, S32 x0, S32 y0, S32 x1, S32 y1, U32 color)
{
    S32 dx = abs(x1 - x0);
    S32 dy = abs(y1 - y0);
    S32 sx = (x0 < x1) ? 1 : -1;
    S32 sy = (y0 < y1) ? 1 : -1;
    S32 err = dx - dy;

    while (1)
    {
        draw_pixel(buffer, x0, y0, color);
        if (x0 == x1 && y0 == y1) break;
        S32 e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 <  dx) { err += dx; y0 += sy; }
    }
}

function void fill_flat_bottom_triangle(game_color_buffer_t* buffer, S32 x0, S32 y0, S32 x1, S32 y1, S32 x2, S32 y2, U32 color)
{
    F32 inv_slope_1 = (F32)(x1 - x0) / (y1 - y0);
    F32 inv_slope_2 = (F32)(x2 - x0) / (y2 - y0);
    F32 x_start = x0;
    F32 x_end   = x0;
    for (S32 y = y0; y <= y2; y++)
    {
        draw_line(buffer, (S32)x_start, y, (S32)x_end, y, color);
        x_start += inv_slope_1;
        x_end   += inv_slope_2;
    }
}

function void fill_flat_top_triangle(game_color_buffer_t* buffer, S32 x0, S32 y0, S32 x1, S32 y1, S32 x2, S32 y2, U32 color)
{
    F32 inv_slope_1 = (F32)(x2 - x0) / (y2 - y0);
    F32 inv_slope_2 = (F32)(x2 - x1) / (y2 - y1);
    F32 x_start = x2;
    F32 x_end   = x2;
    for (S32 y = y2; y >= y0; y--)
    {
        draw_line(buffer, (S32)x_start, y, (S32)x_end, y, color);
        x_start -= inv_slope_1;
        x_end   -= inv_slope_2;
    }
}

function void draw_filled_triangle(game_color_buffer_t* buffer, S32 x0, S32 y0, S32 x1, S32 y1, S32 x2, S32 y2, U32 color)
{
    if (y0 > y1) { Swap(S32, y0, y1); Swap(S32, x0, x1); }
    if (y1 > y2) { Swap(S32, y1, y2); Swap(S32, x1, x2); }
    if (y0 > y1) { Swap(S32, y0, y1); Swap(S32, x0, x1); }

    if (y1 == y2)
    {
        fill_flat_bottom_triangle(buffer, x0, y0, x1, y1, x2, y2, color);
    }
    else if (y0 == y1)
    {
        fill_flat_top_triangle(buffer, x0, y0, x1, y1, x2, y2, color);
    }
    else
    {
        S32 Mx = (x2 - x0) * (y1 - y0) / (y2 - y0) + x0;
        S32 My = y1;
        fill_flat_bottom_triangle(buffer, x0, y0, x1, y1, Mx, My, color);
        fill_flat_top_triangle(buffer, x1, y1, Mx, My, x2, y2, color);
    }
}

function void draw_textured_triangle(
    game_color_buffer_t *buffer,
    S32 x0, S32 y0, F32 u0, F32 v0,
    S32 x1, S32 y1, F32 u1, F32 v1,
    S32 x2, S32 y2, F32 u2, F32 v2,
    U32 *texture)
{
    // TODO:
    // Loop all the pixels of the triangle to render them based on the color
    // that comes from the texture
}

function void draw_triangle(game_color_buffer_t* buffer, S32 x0, S32 y0, S32 x1, S32 y1, S32 x2, S32 y2, U32 color)
{
    draw_line(buffer, x0, y0, x1, y1, color);
    draw_line(buffer, x1, y1, x2, y2, color);
    draw_line(buffer, x2, y2, x0, y0, color);
}

function void draw_rect(game_color_buffer_t* buffer, S32 x, S32 y, U32 width, U32 height, U32 color)
{
    if (x >= buffer->width || x < 0 || y >= buffer->height || y < 0) return;
    for (S32 row = y; row < (S32)(y + height); row++)
    {
        for (S32 col = x; col < (S32)(x + width); col++)
        {
            draw_pixel(buffer, col, row, color);
        }
    }
}

function void clear_color_buffer(game_color_buffer_t* buffer, U32 color)
{
    for (S32 row = 0; row < buffer->height; row++)
    {
        for (S32 col = 0; col < buffer->width; col++)
        {
            draw_pixel(buffer, col, row, color);
        }
    }
}
