#define SDL_MAIN_HANDLED



#include "SDL.h"
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

    bool is_valid;
};

void process_input();
bool initialize_window();
void destroy_window();

global U32 window_width;
global U32 window_height;
global bool is_running = false;
global SDL_Window* window;
global SDL_Renderer* renderer;
global SDL_Texture* color_buffer_texture;
global U32* color_buffer;

global engine_mode_t engine_mode = MODE_NORMAL;
global game_input_t* recorded_inputs;
global U32 recorded_input_count;
global U32 recorded_input_index;
global game_state_t saved_state;

app_t* app;


void render_color_buffer()
{
    SDL_UpdateTexture(
        color_buffer_texture,
        NULL,
        color_buffer,
        window_width * sizeof(U32)
    );

    SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}


win32_game_code load_game_code(char* source_dll_name, char* temp_dll_name)
{
    win32_game_code game_code = {0};
    FILETIME source_last_write_time = get_last_write_time(source_dll_name);
    FILETIME temp_last_write_time = get_last_write_time(temp_dll_name);

    // Only copy and load if the source DLL is newer than the temp DLL
    if (CompareFileTime(&source_last_write_time, &temp_last_write_time) > 0)
    {
        CopyFile(source_dll_name, temp_dll_name, FALSE);
        game_code.game_dll = LoadLibraryA(temp_dll_name);
        if (game_code.game_dll)
        {
            game_code.update_and_render = (game_update_and_render_func)GetProcAddress(
                game_code.game_dll, "game_update_and_render");
            game_code.last_write_time = source_last_write_time;
            game_code.is_valid = (game_code.update_and_render) != NULL;
        }
    }
    else
    {
        game_code.game_dll = LoadLibraryA(temp_dll_name);
        if (game_code.game_dll)
        {
            game_code.update_and_render = (game_update_and_render_func)GetProcAddress(
                game_code.game_dll, "game_update_and_render");
            game_code.last_write_time = source_last_write_time;
            game_code.is_valid = (game_code.update_and_render) != NULL;
        }
    }

    if (!game_code.is_valid)
    {
        game_code.update_and_render = NULL;
    }

    return game_code;
}

void unload_game_code(win32_game_code* game_code)
{
    if (game_code->game_dll)
    {
        FreeLibrary(game_code->game_dll);
        game_code->game_dll = NULL;
    }

    game_code->is_valid = false;
    game_code->update_and_render = NULL;
}

int main()
{
    is_running = initialize_window();

    char* source_dll = "libgame.dll";
    char* temp_dll = "libgame_dll.dll";
    win32_game_code game_code = load_game_code(source_dll, temp_dll);


    arena_t arena = arena_alloc(Gigabytes(64));
    arena_t frame_arena = arena_alloc(Gigabytes(64));

    app = push_array(&arena, app_t, 1);
    app->arena = arena;
    app->frame_arena = frame_arena;
    app->game_state = push_array(&app->arena, game_state_t, 1);
    app->input = push_array(&app->arena, game_input_t, 1);

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

    recorded_inputs = push_array(&app->arena, game_input_t, MAX_INPUT_RECORDS);


    app->color_buffer = &(game_color_buffer_t){color_buffer, window_width, window_height};

    while (is_running)
    {
        FILETIME new_dll_write_time = get_last_write_time(source_dll);
        if (CompareFileTime(&new_dll_write_time, &game_code.last_write_time) != 0)
        {
            unload_game_code(&game_code);
            game_code = load_game_code(source_dll, temp_dll);
            printf("Hot reloaded\n");
        }

        local_persist U32 previous_frame_time = 0;
        {
            U32 time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);
            if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME)
            {
                SDL_Delay(time_to_wait);
            }
            previous_frame_time = SDL_GetTicks();
        }

        process_input();

        if (game_code.is_valid)
        {
            game_code.update_and_render(app);
        }

        if (!game_code.is_valid)
        {
            printf("Error: game_update_and_render function not found in DLL.\n");
        }

        render_color_buffer();

        clear_color_buffer(app->color_buffer, window_width, window_height, 0xFF000000);

        SDL_RenderPresent(renderer);

        arena_clear(&app->frame_arena);
    }

    destroy_window();
}

bool initialize_window()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        fprintf(stderr, "Error initializing SDL \n");
        return false;
    }

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

    if (!window)
    {
        fprintf(stderr, "Error creating SDL window. \n");
        return false;
    }

    renderer = SDL_CreateRenderer(window, -1, 0);

    if (!renderer)
    {
        fprintf(stderr, "Error creating SDL renderer. \n");
        return false;
    }

    return true;
}

void toggleEngineMode()
{
    engine_mode = (engine_mode + 1) % MODE_MAX;

    switch (engine_mode)
    {
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

    default:
        break;
    }
}

void processEventIntoGameInput(SDL_Event* event)
{
    switch (event->type)
    {
    case SDL_KEYDOWN:
        doKeyDown(&event->key);
        break;
    case SDL_KEYUP:
        doKeyUp(&event->key);
        break;
    default: ;
    }
}

void process_input()
{
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        if (engine_mode == MODE_NORMAL || engine_mode == MODE_RECORD)
        {
            processEventIntoGameInput(&event);
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
            color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING,
                                                     window_width, window_height);
            break;
        case SDL_WINDOWEVENT_FOCUS_GAINED:
            SDL_SetWindowOpacity(window, 1.0f);
            break;
        case SDL_WINDOWEVENT_FOCUS_LOST:
            SDL_SetWindowOpacity(window, 0.5f);
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


void destroy_window()
{
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}
