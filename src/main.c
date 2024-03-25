#include "../game/game.h"



#include "Windows.h"


typedef void(*game_update_and_render_func)(game_memory_t *game_memory, game_color_buffer_t *buffer);


global int window_width;
global int window_height;
global bool is_running = false;
global SDL_Window *window;
global SDL_Renderer *renderer;
global SDL_Texture *color_buffer_texture;
global uint32_t *color_buffer;

typedef struct win32_game_code win32_game_code;
struct win32_game_code
{
    HMODULE game_dll;
    FILETIME last_write_time;

    game_update_and_render_func update_and_render;

    bool is_valid;
};


void render_color_buffer(  )
{
    SDL_UpdateTexture(
            color_buffer_texture,
            NULL,
            color_buffer,
            ( int ) ( window_width * sizeof( uint32_t ) )
    );


    SDL_RenderCopy( renderer, color_buffer_texture, NULL, NULL );
}

void clear_color_buffer( uint32_t color )
{
    for( int y = 0; y < window_height; ++y )
    {
        for( int x = 0; x < window_width; ++x )
        {
            color_buffer[ ( y * window_width ) + x ] = color;
        }
    }
}


FILETIME get_last_write_time(char *filename)
{
    FILETIME LastWriteTime = {0};
    WIN32_FILE_ATTRIBUTE_DATA data;
    if(GetFileAttributesEx(filename, GetFileExInfoStandard, &data))
    {
        LastWriteTime = data.ftLastWriteTime;
    }

    return LastWriteTime;
}

win32_game_code load_game_code(char *source_dll_name, char *temp_dll_name)
{
    win32_game_code game_code = {0};
    FILETIME source_last_write_time = get_last_write_time(source_dll_name);
    FILETIME temp_last_write_time = get_last_write_time(temp_dll_name);

    // Only copy and load if the source DLL is newer than the temp DLL
    if( CompareFileTime(&source_last_write_time, &temp_last_write_time) > 0)
    {
        CopyFile(source_dll_name, temp_dll_name, FALSE);
        game_code.game_dll = LoadLibraryA(temp_dll_name);
        if(game_code.game_dll)
        {
            game_code.update_and_render = (game_update_and_render_func ) GetProcAddress(game_code.game_dll,"game_update_and_render");
            game_code.last_write_time = source_last_write_time;
            game_code.is_valid = (game_code.update_and_render) != NULL;
        }
    }
    else
    {
        game_code.game_dll = LoadLibraryA(temp_dll_name);
        if(game_code.game_dll)
        {
            game_code.update_and_render = (game_update_and_render_func ) GetProcAddress(game_code.game_dll,"game_update_and_render");
            game_code.last_write_time = source_last_write_time;
            game_code.is_valid = (game_code.update_and_render) != NULL;
        }
    }

    if(!game_code.is_valid)
    {
        game_code.update_and_render = NULL;
    }

    return game_code;
}


void unload_game_code(win32_game_code *game_code)
{
    if(game_code->game_dll)
    {
        FreeLibrary(game_code->game_dll);
        game_code->game_dll = NULL;
    }

    game_code->is_valid = false;
    game_code->update_and_render = NULL;
}


void setup( );
void process_input(  );
bool initialize_window( );
void destroy_window(  );
void free_resources( );


int main( )
{
    is_running = initialize_window( );

    char *source_dll = "game.dll";
    char *temp_dll = "game_dll.dll";
    win32_game_code game_code = load_game_code(source_dll, temp_dll);


    game_memory_t game_memory;
    memset(&game_memory, 0, sizeof(game_memory));
    game_memory.size = Megabytes(64);
    game_memory.memory = malloc(game_memory.size);

    setup( );

    if(game_memory.memory == NULL) {
        printf("malloc failed\n");
        exit(1);
    }

    while( is_running )
    {
        FILETIME new_dll_write_time = get_last_write_time(source_dll);
        if( CompareFileTime(&new_dll_write_time, &game_code.last_write_time) != 0)
        {
            unload_game_code(&game_code);
            game_code = load_game_code(source_dll, temp_dll);
            printf("Hot reloaded!\n");
        }

        process_input(  );

        if(game_code.is_valid)
        {
            game_color_buffer_t game_color_buffer;
            game_color_buffer.memory = color_buffer;
            game_color_buffer.width = window_width;
            game_color_buffer.height = window_height;
            game_code.update_and_render(&game_memory, &game_color_buffer);
        }

        render_color_buffer();
        clear_color_buffer(0xFF000000);

        SDL_RenderPresent(renderer);
    }


    free(game_memory.memory);

    destroy_window(  );
    free_resources(  );
    return 0;
}


bool initialize_window(  )
{
    if( SDL_Init( SDL_INIT_EVERYTHING ) != 0 )
    {
        fprintf( stderr, "Error initializing SDL \n" );
        return false;
    }

    window_width = 800; // Desired window width
    window_height = 600; // Desired window height*/

    window = SDL_CreateWindow(
            "3D_Graphics", // You can set your window title here
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            window_width,
            window_height,
            SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_RESIZABLE
    );


    if( !window )
    {
        fprintf( stderr, "Error creating SDL window. \n" );
        return false;
    }

    renderer = SDL_CreateRenderer( window, -1, 0 );

    if( !renderer )
    {
        fprintf( stderr, "Error creating SDL renderer. \n" );
        return false;
    }

    return true;
}


void setup(   )
{
    color_buffer = ( uint32_t * ) malloc( sizeof( uint32_t ) * window_width * window_height );

    color_buffer_texture = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_ARGB8888,
            SDL_TEXTUREACCESS_STREAMING,
            window_width,
            window_height
    );
}

void process_input() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                is_running = false;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_ESCAPE) is_running = false;
                break;
            case SDL_WINDOWEVENT:
                switch (event.window.event) {
                    case SDL_WINDOWEVENT_RESIZED:
                        window_width = event.window.data1;
                        window_height = event.window.data2;

                        // Reallocate the color memory to match the new size
                        free(color_buffer);
                        color_buffer = (uint32_t *)malloc(sizeof(uint32_t) * window_width * window_height);

                        // Recreate the texture with the new dimensions
                        SDL_DestroyTexture(color_buffer_texture);
                        color_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, window_width, window_height);
                        break;
                    case SDL_WINDOWEVENT_FOCUS_GAINED:
                        SDL_SetWindowOpacity(window, 1.0f);
                        break;
                    case SDL_WINDOWEVENT_FOCUS_LOST:
                        SDL_SetWindowOpacity(window, 0.5f);
                        break;
                }
                break;
        }
    }
}


void free_resources(  )
{
    free( color_buffer );
}

void destroy_window(  )
{
    SDL_DestroyRenderer( renderer );
    SDL_DestroyWindow( window );
    SDL_Quit( );
}
