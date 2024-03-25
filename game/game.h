//
// Created by Karim on 3/22/2024.
//

#ifndef INC_3D_GRAPHICS_GAME_H
#define INC_3D_GRAPHICS_GAME_H

#define SDL_MAIN_HANDLED
#include "SDL.h"
#include "stdbool.h"
#include "../include/mesh.h"

#ifdef __cplusplus
#define root_function extern "C" __declspec(dllexport)
#else
#define root_function __declspec(dllexport)
#endif

//~ rjf: Base-Types
typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;
typedef float    F32;
typedef double   F64;

#define local_persist static
#define function static
#define global static

#define Bytes(n)      (n)
#define Kilobytes(n)  (n << 10)
#define Megabytes(n)  (n << 20)
#define Gigabytes(n)  (((U64)n) << 30)
#define Terabytes(n)  (((U64)n) << 40)

typedef struct game_color_buffer_t game_color_buffer_t;
struct game_color_buffer_t
{
    uint32_t *memory;
    int width;
    int height;
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
    SDL_Rect player;
    S32 offset;
    mesh_t *mesh;
};

root_function void game_update_and_render( game_memory_t *game_memory, game_color_buffer_t *buffer);

#endif //INC_3D_GRAPHICS_GAME_H
