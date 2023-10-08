//
// Created by Karim on 2023-10-07.
//
#ifndef INC_3D_GRAPHICS_DISPLAY_H
#define INC_3D_GRAPHICS_DISPLAY_H

#include "stdbool.h"
#include "SDL.h"

#define FPS 30
#define FRAME_TARGET_TIME (1000/FPS)

extern SDL_Window *window;
extern SDL_Renderer *renderer;
extern uint32_t *color_buffer;
extern SDL_Texture *color_buffer_texture;
extern int window_width;
extern int window_height;

bool initialize_window( void );
void draw_grid( );
void draw_pixel(int x, int y, uint32_t color);
void draw_rect( int x, int y, int width, int height, uint32_t color );
void render_color_buffer( );
void clear_color_buffer( uint32_t color );
void destroy_window( );

#endif //INC_3D_GRAPHICS_DISPLAY_H
