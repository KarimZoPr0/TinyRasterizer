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
    U32* memory;
    U32 width;
    U32 height;
};

typedef struct game_state_t game_state_t;
struct game_state_t
{
    rect_t player;
    S32 offset;
    mesh_t* mesh;
};

typedef enum cull_mode cull_mode;
enum cull_mode
{
    CULL_NONE,
    CULL_BACKFACE
};

typedef enum render_mode render_mode;
enum render_mode
{
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE
};

typedef struct mouse_t mouse_t;
struct mouse_t
{
    S32 x;
    S32 y;
    vec2_t scroll;
};

typedef enum {
    KEY_UNKNOWN = 0,

    KEY_A,
    KEY_B,
    KEY_C,
    KEY_D,
    KEY_E,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_I,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_M,
    KEY_N,
    KEY_O,
    KEY_P,
    KEY_Q,
    KEY_R,
    KEY_S,
    KEY_T,
    KEY_U,
    KEY_V,
    KEY_W,
    KEY_X,
    KEY_Y,
    KEY_Z,

    KEY_1,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,

    KEY_F1,
    KEY_F2,
    KEY_F3,
    KEY_F4,
    KEY_F5,
    KEY_F6,
    KEY_F7,
    KEY_F8,
    KEY_F9,
    KEY_F10,
    KEY_F11,
    KEY_F12,

    KEY_UP,
    KEY_DOWN,
    KEY_LEFT,
    KEY_RIGHT,

    KEY_RETURN,
    KEY_ESCAPE,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_SPACE,

    KEY_MINUS,
    KEY_EQUALS,
    KEY_LEFT_BRACKET,
    KEY_RIGHT_BRACKET,
    KEY_BACKSLASH,
    KEY_SEMICOLON,
    KEY_APOSTROPHE,
    KEY_GRAVE,
    KEY_COMMA,
    KEY_PERIOD,
    KEY_SLASH,

    KEY_CAPSLOCK,
    KEY_PRINTSCREEN,
    KEY_SCROLLLOCK,
    KEY_PAUSE,
    KEY_INSERT,
    KEY_HOME,
    KEY_PAGEUP,
    KEY_DELETE,
    KEY_END,
    KEY_PAGEDOWN,

    KEY_COUNT
} KeyCode;

typedef struct game_input_t game_input_t;
struct game_input_t
{
    U32 keyboard[KEY_COUNT];
    mouse_t mouse;
};


typedef struct app_t app_t;
struct app_t
{
    Arena arena;
    Arena frame_arena;

    game_color_buffer_t *color_buffer;
    game_state_t *game_state;
    game_input_t *input;

    cull_mode cull_mode;
    render_mode render_mode;


    B32 is_initialized;
};


root_function void game_update_and_render(app_t* app);

#endif //INC_3D_GRAPHICS_GAME_H
