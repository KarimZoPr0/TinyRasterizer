//
// Created by Karim on 3/22/2024.
//

#ifndef INC_3D_GRAPHICS_GAME_H
#define INC_3D_GRAPHICS_GAME_H
#define SDL_MAIN_HANDLED


#ifdef __cplusplus
#define root_function extern "C" __declspec(dllexport)
#else
#define root_function __declspec(dllexport)
#endif

typedef struct game_color_buffer_t game_color_buffer_t;
struct game_color_buffer_t
{
    U32 *memory;
    U32 width;
    U32 height;
};

typedef struct game_memory_t game_memory_t;
struct game_memory_t
{
    void* memory;
    U32 size;
    B32 is_initialized;
};

typedef struct game_state_t game_state_t;
struct game_state_t
{
    rect_t player;
    S32 offset;
    mesh_t *mesh;
};

// TODO: Replace this with keyboard array and pass a keyboard from the platform layer
typedef struct game_input_t game_input_t;
struct game_input_t
{
    U32 up;
    U32 down;
    U32 left;
    U32 right;
};

root_function void game_update_and_render( game_memory_t *game_memory, game_input_t *input, game_color_buffer_t *buffer);

#endif //INC_3D_GRAPHICS_GAME_H
