//
// Created by Karim on 2023-10-07.
//
#ifndef INC_3D_GRAPHICS_DISPLAY_H
#define INC_3D_GRAPHICS_DISPLAY_H

#define FPS 60
#define FRAME_TARGET_TIME (1000/FPS)

void D_Grid2D( game_color_buffer_t *buffer);
void D_Pixel2D( game_color_buffer_t *buffer, S32 x, S32 y, U32 color);
void D_Line2D( game_color_buffer_t *buffer, S32 x0, S32 y0, S32 x1, S32 y1, U32 color);
void D_Triangle2D( game_color_buffer_t *buffer, S32 x0, S32 y0, S32 x1, S32 y1, S32 x2, S32 y2, U32 color);
void D_Rect2D( game_color_buffer_t *buffer, S32 x, S32 y, U32 width, U32 height, U32 color );
// void render_color_buffer( uint32_t *color_buffer);
// void clear_color_buffer(uint32_t *color_buffer, uint32_t color );

#endif //INC_3D_GRAPHICS_DISPLAY_H
