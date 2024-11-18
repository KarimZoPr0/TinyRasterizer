//
// Created by Karim on 2023-10-07.
//


void draw_grid(game_color_buffer_t* buffer)
{
    for (U32 y = 0; y < buffer->height; y += 10)
    {
        for (U32 x = 0; x < buffer->width; x += 10)
        {
            if (x % 10 == 0 || y % 10 == 0)
            {
                buffer->memory[(y * buffer->width) + x] = 0xFF333333;
            }
        }
    }
}

void draw_pixel(game_color_buffer_t* buffer, S32 x, S32 y, U32 color)
{
    if (x >= buffer->width || x < 0 || y >= buffer->height || y < 0) return;
    buffer->memory[(y * buffer->width) + x] = color;
}

void draw_triangle(game_color_buffer_t* buffer, S32 x0, S32 y0, S32 x1, S32 y1, S32 x2, S32 y2, U32 color)
{
    draw_line(buffer, x0, y0, x1, y1, color);
    draw_line(buffer, x1, y1, x2, y2, color);
    draw_line(buffer, x2, y2, x0, y0, color);
}

void fill_flat_bottom_triangle(game_color_buffer_t* buffer, S32 x0, S32 y0, S32 x1, S32 y1, S32 x2, S32 y2, U32 color)
{
    // Find the two slopes (two triangle legs)
    F32 inv_slope_1 = (F32)(x1-x0)/(y1-y0);
    F32 inv_slope_2 = (F32)(x2-x0)/(y2-y0);

    // Start x_start and x_end from the top vertex (x0,y0)
    F32 x_start = x0;
    F32 x_end = x0;

    // Loop all the scanlines from top to bottom
    for(U32 y = y0; y <= y2; ++y)
    {
        draw_line(buffer, x_start, y, x_end, y, color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;
    }
}

void fill_flat_top_triangle(game_color_buffer_t* buffer, S32 x0, S32 y0, S32 x1, S32 y1, S32 x2, S32 y2, U32 color)
{
    // Find the two slopes (two triangle legs)
    F32 inv_slope_1 = (F32)(x2-x0)/(y2-y0);
    F32 inv_slope_2 = (F32)(x2-x1)/(y2-y1);

    // Start x_start and x_end from the bottom vertex (x2,y2)
    F32 x_start = x2;
    F32 x_end = x2;

    // Loop all the scanlines from top to bottom
    for(S32 y = y2; y >= y0; y--)
    {
        draw_line(buffer, x_start, y, x_end, y, color);
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }
}


void draw_filled_triangle(game_color_buffer_t* buffer, S32 x0, S32 y0, S32 x1, S32 y1, S32 x2, S32 y2, U32 color)
{
    // We need to sort vertices by y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1)
    {
        Swap(S32, y0, y1);
        Swap(S32, x0, x1);
    }

    if (y1 > y2)
    {
        Swap(S32, y1, y2);
        Swap(S32, x1, x2);
    }

    if (y0 > y1)
    {
        Swap(S32, y0, y1);
        Swap(S32, x0, x1);
    }

    if(y1 == y2)
    {
        fill_flat_bottom_triangle(buffer, x0, y0, x1, y1, x2, y2, color);
    }
    else if(y0 == y1)
    {
        fill_flat_top_triangle(buffer, x0, y0, x1, y1, x2, y2, color);
    }
    else
    {
        // Calculate the new vertex (Mx, My) using triangle similarity
        S32 Mx = (x2 - x0) * (y1 - y0) / (y2 - y0) + x0;
        S32 My = y1;

        fill_flat_bottom_triangle(buffer, x0, y0, x1, y1, Mx, My, color);
        fill_flat_top_triangle(buffer, x1, y1, Mx, My, x2, y2, color);
    }

}

void draw_line(game_color_buffer_t* buffer, S32 x0, S32 y0, S32 x1, S32 y1, U32 color)
{
    S32 dx = abs(x1 - x0);
    S32 dy = abs(y1 - y0);
    S32 sx = x0 < x1 ? 1 : -1;
    S32 sy = y0 < y1 ? 1 : -1;
    S32 err = dx - dy;

    while (1)
    {
        draw_pixel(buffer, x0, y0, color);

        if (x0 == x1 && y0 == y1)
            break;

        S32 e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x0 += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}


void draw_rect(game_color_buffer_t* buffer, S32 x, S32 y, U32 width, U32 height, U32 color)
{
    if (x >= buffer->width || x < 0 || y >= buffer->height || y < 0) return;
    for (U32 row = y; row < y + height; ++row)
    {
        for (U32 col = x; col < x + width; ++col)
        {
            draw_pixel(buffer, col, row, color);
        }
    }
}

void clear_color_buffer(game_color_buffer_t* buffer, U32 width, U32 height, U32 color)
{
    for (int row = 0; row < height; ++row)
    {
        for (int col = 0; col < width; ++col)
        {
            draw_pixel(buffer, col, row, color);
        }
    }
}
