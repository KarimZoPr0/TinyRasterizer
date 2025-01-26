#define SDL_MAIN_HANDLED

#include "external/spall.h"

#include "SDL2/SDL.h"
#include "base/base_inc.h"
#include "os/os_inc.h"
#include "system/system_inc.h"

#include "system/input.h"
#include "game/game.h"
#include "system/input.c"

#include "base/base_inc.c"
#include "os/os_inc.c"
#include "system/system_inc.c"

#define FPS 60
#define FRAME_TARGET_TIME (1000/FPS)
#define RECORDING_DURATION_SECONDS (10 * 60)
#define MAX_INPUT_RECORDS (FPS * RECORDING_DURATION_SECONDS)

typedef enum
{
    MODE_NORMAL,
    MODE_RECORD,
    MODE_PLAYBACK,
    MODE_MAX
} engine_mode_t;

typedef void (*game_update_and_render_func)(app_t* app);

typedef struct win32_game_code win32_game_code;

struct win32_game_code
{
    HMODULE game_dll;
    FILETIME last_write_time;

    game_update_and_render_func update_and_render;

    B32 is_valid;
};

global U32 window_width;
global U32 window_height;
global B32 is_running = 1;
global SDL_Window* window;
global SDL_Renderer* renderer;
global SDL_Texture* color_buffer_texture;
global U32* color_buffer;

global engine_mode_t engine_mode = MODE_NORMAL;
global game_input_t* recorded_inputs;
global U32 recorded_input_count;
global U32 recorded_input_index;
global game_state_t saved_state;

global SpallProfile spall_ctx;
global SpallBuffer spall_buffer;

app_t* app;


function void toggleEngineMode()
{
    engine_mode = (engine_mode + 1) % MODE_MAX;

    switch (engine_mode)
    {
    default:
        break;
    case MODE_NORMAL:
        memset(app->input, 0, sizeof(game_input_t));
        break;
    case MODE_RECORD:
        recorded_input_count = recorded_input_index = 0;
        memcpy(&saved_state, app->game_state, sizeof(game_state_t));
        break;
    case MODE_PLAYBACK:
        memcpy(app->game_state, &saved_state, sizeof(game_state_t));
        break;
    }
}

function win32_game_code load_game_code(char* source_dll_name, char* temp_dll_name)
{
    win32_game_code game_code = {0};
    FILETIME source_last_write_time = get_last_write_time(source_dll_name);
    FILETIME temp_last_write_time = get_last_write_time(temp_dll_name);

    //- karim: only copy and load if the source DLL is newer than the temp DLL
    if (CompareFileTime(&source_last_write_time, &temp_last_write_time) > 0)
    {
        CopyFile(source_dll_name, temp_dll_name, FALSE);
    }

    game_code.game_dll = LoadLibraryA(temp_dll_name);
    if (game_code.game_dll)
    {
        game_code.update_and_render = (game_update_and_render_func)GetProcAddress(game_code.game_dll, "game_update_and_render");
        game_code.last_write_time = source_last_write_time;
        game_code.is_valid = game_code.update_and_render != 0;
    }

    return game_code;
}

int main()
{
    //- karim: SDL initialization
    SDL_Init(SDL_INIT_EVERYTHING);
    window_width = 800;
    window_height = 600;
    window = SDL_CreateWindow(
        "Tiny Rasterizer",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        window_width,
        window_height,
        SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_RESIZABLE
    );
    renderer = SDL_CreateRenderer(window, -1, 0);

    //- karim: load game code
    char* source_dll = "libgame.dll";
    char* temp_dll = "libgame_dll.dll";
    win32_game_code game_code = load_game_code(source_dll, temp_dll);

    //- karim: set up state
    game_state_t* state = {0};
    {
        arena_t arena = arena_alloc(Gigabytes(64));
        arena_t frame_arena = arena_alloc(Gigabytes(8));
        arena_t meshes_arena = arena_alloc(Gigabytes(8));
        arena_t vertex_chunk_arena = arena_alloc(Gigabytes(8));
        arena_t face_chunk_arena = arena_alloc(Gigabytes(8));

        state = push_array(&arena, game_state_t, 1);
        state->arena = arena;
        state->frame_arena = frame_arena;
        state->meshes_arena = meshes_arena;
        state->vertex_chunk_arena = vertex_chunk_arena;
        state->face_chunk_arena = face_chunk_arena;
    }

    //- karim: set up app
    {
        arena_t arena = arena_alloc(Gigabytes(64));
        app = push_array(&arena, app_t, 1);
        app->arena = arena;
        app->game_state = state;

        //-karim: set up input
        app->input = push_array(&app->arena, game_input_t, 1);
        recorded_inputs = push_array(&app->arena, game_input_t, MAX_INPUT_RECORDS);

        //- karim: set up color buffer
        SDL_DisplayMode display_mode;
        SDL_GetCurrentDisplayMode(0, &display_mode);
        color_buffer = push_array(&app->arena, U32, display_mode.w * display_mode.h);
        color_buffer_texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            window_width,
            window_height
        );
        app->color_buffer = push_array(&app->arena, game_color_buffer_t, 1);
        app->color_buffer->memory = color_buffer;
        app->color_buffer->width = window_width;
        app->color_buffer->height = window_height;
    }

    //- karim: set up spall profiler
    app->spall_ctx = spall_init_file("../TinyRasterizer.spall", 1);
    U32 buffer_size = 1 * 1024 * 1024;
    U8* buffer = push_array(&app->arena, U8, buffer_size);
    app->spall_buffer.length = buffer_size;
    app->spall_buffer.data = &buffer;
    spall_buffer_init(&app->spall_ctx, &app->spall_buffer);

    //- karim: main loop
    U32 previous_frame_time = 0;
    U32 time_to_wait = 0;
    while (is_running)
    {

        //- karim: cap frame rate
        time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
        if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
        {
            SDL_Delay(time_to_wait);
        }

        app->delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;

        previous_frame_time = SDL_GetTicks();

        //- karim: Hotreload
        FILETIME new_dll_write_time = get_last_write_time(source_dll);
        if (CompareFileTime(&new_dll_write_time, &game_code.last_write_time) != 0)
        {
            //- karim: unload game code
            if (game_code.game_dll)
            {
                FreeLibrary(game_code.game_dll);
                game_code.game_dll = 0;
            }

            //- karim: load game code
            game_code.is_valid = false;
            game_code.update_and_render = 0;
            game_code = load_game_code(source_dll, temp_dll);
        }

        //- karim: process input
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                if (engine_mode == MODE_NORMAL || engine_mode == MODE_RECORD)
                {
                    switch (event.type)
                    {
                    default: ;
                    case SDL_KEYDOWN:
                        doKeyDown(&event.key);
                        break;
                    case SDL_KEYUP:
                        doKeyUp(&event.key);
                        break;
                    }
                }

                if (event.type == SDL_KEYDOWN)
                {
                    if (event.key.repeat == 0 && event.key.keysym.scancode == SDL_SCANCODE_L)
                    {
                        toggleEngineMode();
                    }
                }
                else if (event.type == SDL_QUIT)
                {
                    is_running = false;
                }

                switch (event.window.event)
                {
                case SDL_WINDOWEVENT_RESIZED:
                    app->color_buffer->width = window_width = event.window.data1;
                    app->color_buffer->height = window_height = event.window.data2;

                    SDL_DestroyTexture(color_buffer_texture);
                    color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                                             SDL_TEXTUREACCESS_STREAMING,
                                                             window_width, window_height);
                    break;
                case SDL_WINDOWEVENT_FOCUS_GAINED:
                    // SDL_SetWindowOpacity(window, 1.0f);
                    break;
                case SDL_WINDOWEVENT_FOCUS_LOST:
                    // SDL_SetWindowOpacity(window, 0.5f);
                    break;
                default:
                    break;
                }
            }

            if (engine_mode == MODE_RECORD)
            {
                if (recorded_input_count < MAX_INPUT_RECORDS)
                {
                    recorded_inputs[recorded_input_count++] = *app->input;
                }
                else
                {
                    printf("Recorded input buffer is full.\n");
                    engine_mode = MODE_RECORD;
                    toggleEngineMode();
                }
            }

            if (engine_mode == MODE_PLAYBACK)
            {
                if (recorded_input_index < recorded_input_count)
                {
                    *app->input = recorded_inputs[recorded_input_index++];
                }
                else if (recorded_input_index >= recorded_input_count)
                {
                    recorded_input_index = 0;
                    memcpy(app->game_state, &saved_state, sizeof(game_state_t));
                }
            }
        }


        //- karim: update & render
        if (game_code.is_valid)
        {
            game_code.update_and_render(app);
        }

        //- karim: render
        {
            //- karim: render color buffer
            SDL_UpdateTexture(color_buffer_texture, 0, color_buffer, window_width * sizeof(U32));
            SDL_RenderCopy(renderer, color_buffer_texture, 0, 0);

            //- karim: clear & present
            clear_color_buffer(app->color_buffer, 0xFF000000);
            SDL_RenderPresent(renderer);
        }

        //- karim: flush spall buffer
        spall_buffer_quit(&app->spall_ctx, &app->spall_buffer);

        //- karim: clear frame arena
        arena_clear(&state->frame_arena);
    }

    //- karim: clean up
    spall_quit(&spall_ctx);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
