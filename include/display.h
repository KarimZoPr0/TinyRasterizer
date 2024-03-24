//
// Created by Karim on 2023-10-07.
//
#ifndef INC_3D_GRAPHICS_DISPLAY_H
#define INC_3D_GRAPHICS_DISPLAY_H

#define FPS 60
#define FRAME_TARGET_TIME (1000/FPS)

void D_Grid2D( game_color_buffer_t *buffer);
void D_Pixel2D( game_color_buffer_t *buffer, int x, int y, uint32_t color);
void D_Line2D( game_color_buffer_t *buffer, int x0, int y0, int x1, int y1, uint32_t color);
void D_Triangle2D( game_color_buffer_t *buffer, int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
void D_Rect2D( game_color_buffer_t *buffer, int x, int y, int width, int height, uint32_t color );
// void render_color_buffer( uint32_t *color_buffer);
// void clear_color_buffer(uint32_t *color_buffer, uint32_t color );

#endif //INC_3D_GRAPHICS_DISPLAY_H
