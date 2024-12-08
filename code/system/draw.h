//
// Created by Karim on 2023-10-07.
//
#ifndef INC_3D_GRAPHICS_DISPLAY_H
#define INC_3D_GRAPHICS_DISPLAY_H

#define FPS 60
#define FRAME_TARGET_TIME (1000/FPS)

typedef struct game_color_buffer_t game_color_buffer_t;
struct game_color_buffer_t
{
    U32* memory;
    U32 width;
    U32 height;
};

function void draw_grid( game_color_buffer_t *buffer);
function void draw_pixel( game_color_buffer_t *buffer, S32 x, S32 y, U32 color);
function void draw_line( game_color_buffer_t *buffer, S32 x0, S32 y0, S32 x1, S32 y1, U32 color);
function void draw_triangle( game_color_buffer_t *buffer, S32 x0, S32 y0, S32 x1, S32 y1, S32 x2, S32 y2, U32 color);
function void draw_filled_triangle( game_color_buffer_t *buffer, S32 x0, S32 y0, S32 x1, S32 y1, S32 x2, S32 y2, U32 color);
function void draw_rect( game_color_buffer_t *buffer, S32 x, S32 y, U32 width, U32 height, U32 color );
function void clear_color_buffer(game_color_buffer_t* buffer, U32 color);
// void render_color_buffer( uint32_t *color_buffer);
// void clear_color_buffer(uint32_t *color_buffer, uint32_t color );

#endif //INC_3D_GRAPHICS_DISPLAY_H
