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

typedef U64 cull_flags;

enum cull_flags
{
    CULL_FLAG_BACKFACE = (1 << 0),
};

typedef U64 render_flags;

enum render_flags
{
    RENDER_FLAG_VERTEX = (1 << 0),
    RENDER_FLAG_WIRE = (1 << 1),
    RENDER_FLAG_FILL = (1 << 2),
    RENDER_FLAG_TEXTURE = (1 << 3),
};

typedef struct mouse_t mouse_t;

struct mouse_t
{
    S32 x;
    S32 y;
    vec2_t scroll;
};

typedef enum
{
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


typedef struct entity_t entity_t;

struct entity_t
{
    mesh_t* mesh;

    // transform prop
    vec3_t translation;
    vec3_t scale;
    vec3_t rotation;
};

typedef game_state_t game_state_t;

struct game_state_t
{
    arena_t* arena;

    entity_t e1;
    entity_t nil_entity;

    arena_t* meshes_arena;
    mesh_t* first_free_mesh;
    mesh_table_t* mesh_table;

    arena_t* vertex_chunk_arena;
    vertex_chunk_node_t* first_free_vertex_chunk;
    arena_t* face_chunk_arena;
    face_chunk_node_t* first_free_face_chunk;

    vec3_t rotation;
};

typedef struct camera_t camera_t;

struct camera_t
{
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    F32 yaw;
};


typedef struct app_t app_t;

struct app_t
{
    arena_t* arena;
    arena_t* frame_arena;

    game_color_buffer_t* color_buffer;
    game_state_t* game_state;
    game_input_t* input;

    cull_flags cull_flags;
    render_flags render_flags;

    SpallProfile spall_ctx;
    SpallBuffer spall_buffer;

    B32 is_initialized;
    F32 delta_time;
};


root_function void game_update_and_render(app_t* app);

#endif //INC_3D_GRAPHICS_GAME_H
