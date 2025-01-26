//
// Created by Karim on 2024-11-12.
//
// Vector 2 functions
function F32 vec2_length(vec2_t v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
}

function vec2_t vec2_normalize(vec2_t v)
{
    vec2_t result = {0};
    F32 length = vec2_length(v);

    if (length > 0.0f)
    {
        result.x = v.x / length;
        result.y = v.y / length;
    }
    return result;
}

function vec2_t vec2_add(vec2_t a, vec2_t b)
{
    vec2_t result = {
        a.x + b.x,
        a.y + b.y
    };
    return result;
}

function vec2_t vec2_sub(vec2_t a, vec2_t b)
{
    vec2_t result = {
        a.x - b.x,
        a.y - b.y
    };
    return result;
}

function vec2_t vec2_mul(vec2_t a, F32 factor)
{
    vec2_t result = {
        a.x * factor,
        a.y * factor
    };
    return result;
}

function vec2_t vec2_div(vec2_t a, F32 factor)
{
    vec2_t result = {
        a.x / factor,
        a.y / factor
    };
    return result;
}

function F32 vec2_dot(vec2_t a, vec2_t b)
{
    return a.x * b.x + a.y * b.y;
}


// Vector 3 functions
function F32 vec3_length(vec3_t v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
}

function vec3_t vec3_normalize(vec3_t v)
{
    vec3_t result = {0};
    F32 length = vec3_length(v);
    if (length > 0.0f)
    {
        result.x = v.x / length;
        result.y = v.y / length;
        result.z = v.z / length;
    }
    return result;
}

function vec3_t vec3_add(vec3_t a, vec3_t b)
{
    vec3_t result = {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
    return result;
}

function vec3_t vec3_sub(vec3_t a, vec3_t b)
{
    vec3_t result = {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
    return result;
}

function vec3_t vec3_mul(vec3_t a, F32 factor)
{
    vec3_t result = {
        a.x * factor,
        a.y * factor,
        a.z * factor
    };
    return result;
}

function vec3_t vec3_div(vec3_t a, F32 factor)
{
    vec3_t result = {
        a.x / factor,
        a.y / factor,
        a.z / factor
    };
    return result;
}

function vec3_t vec3_cross(vec3_t a, vec3_t b)
{
    vec3_t result =
    {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
    return result;
}

function F32 vec3_dot(vec3_t a, vec3_t b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

function vec3_t vec3_rotate_x(vec3_t v, F32 angle)
{
    vec3_t rotated_vector = {
        .x = v.x,
        .y = v.y * cosf(angle) - v.z * sinf(angle),
        .z = v.y * sinf(angle) + v.z * cosf(angle)
    };
    return rotated_vector;
}

function vec3_t vec3_rotate_y(vec3_t v, F32 angle)
{
    vec3_t rotated_vector = {
        .x = v.x * cosf(angle) - v.z * sinf(angle),
        .y = v.y,
        .z = v.x * sinf(angle) + v.z * cosf(angle)
    };
    return rotated_vector;
}

function vec3_t vec3_rotate_z(vec3_t v, F32 angle)
{
    vec3_t rotated_vector = {
        .x = v.x * cosf(angle) - v.y * sinf(angle),
        .y = v.x * sinf(angle) + v.y * cosf(angle),
        .z = v.z
    };
    return rotated_vector;
}

//- karim: vector 4 functions
function vec4_t vec4_from_vec3(vec3_t v)
{
    vec4_t result = {v.x, v.y, v.z, 1};
    return result;
}

function vec3_t vec3_from_vec4(vec4_t v)
{
    vec3_t result = {v.x, v.y, v.z};
    return result;
}

// Projection
function vec2_t project(vec3_t point, F32 fov_factor)
{
    const vec2_t projected_point = {
        point.x * fov_factor / point.z,
        point.y * fov_factor / point.z
    };
    return projected_point;
}

//- karim: matrix functions
function mat4_t mat4_identity(void)
{
    mat4_t m = {
        {
            {1, 0, 0, 0},
            {0, 1, 0, 0},
            {0, 0, 1, 0},
            {0, 0, 0, 1},
        }
    };
    return m;
}

function mat4_t mat4_make_translation(F32 tx, F32 ty, F32 tz)
{
    mat4_t m = mat4_identity();
    m.m[0][3] = tx;
    m.m[1][3] = ty;
    m.m[2][3] = tz;
    return m;
}

function mat4_t mat4_make_scale(F32 sx, F32 sy, F32 sz)
{
    mat4_t m = mat4_identity();
    m.m[0][0] = sx;
    m.m[1][1] = sy;
    m.m[2][2] = sz;
    return m;
}

function mat4_t mat4_make_rotation_x(F32 angle)
{
    F32 c = cosf(angle);
    F32 s = sinf(angle);
    mat4_t m = mat4_identity();
    m.m[1][1] = c;
    m.m[1][2] = -s;
    m.m[2][1] = s;
    m.m[2][2] = c;
    return m;
}

function mat4_t mat4_make_rotation_y(F32 angle)
{
    F32 c = cosf(angle);
    F32 s = sinf(angle);
    mat4_t m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][2] = s;
    m.m[2][0] = -s;
    m.m[2][2] = c;
    return m;
}

function mat4_t mat4_make_rotation_z(F32 angle)
{
    F32 c = cosf(angle);
    F32 s = sinf(angle);
    mat4_t m = mat4_identity();
    m.m[0][0] = c;
    m.m[0][1] = -s;
    m.m[1][0] = s;
    m.m[1][1] = c;
    return m;
}

function vec4_t mat4_mul_vec4(mat4_t m, vec4_t v)
{
    vec4_t result = {0};

    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3] * v.w;
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3] * v.w;
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3] * v.w;
    result.w = m.m[3][0] * v.x + m.m[3][1] * v.y + m.m[3][2] * v.z + m.m[3][3] * v.w;

    return result;
}

function vec3_t mat4x3_mul_vec3(mat4_t m, vec3_t v)
{
    vec3_t result = {0};

    result.x = m.m[0][0] * v.x + m.m[0][1] * v.y + m.m[0][2] * v.z + m.m[0][3];
    result.y = m.m[1][0] * v.x + m.m[1][1] * v.y + m.m[1][2] * v.z + m.m[1][3];
    result.z = m.m[2][0] * v.x + m.m[2][1] * v.y + m.m[2][2] * v.z + m.m[2][3];

    return result;
}

function mat4_t mat4_mul_mat4(mat4_t a, mat4_t b)
{
    mat4_t m = {0};
    for (U64 i = 0; i < 4; ++i)
    {
        for (U64 j = 0; j < 4; ++j)
        {
            m.m[i][j] = a.m[i][0] * b.m[0][j] + a.m[i][1] * b.m[1][j] + a.m[i][2] * b.m[2][j] + a.m[i][3] * b.m[3][j];
        }
    }
    return m;
}

function mat4_t mat4_make_perspective(F32 fov, F32 aspect, F32 znear, F32 zfar)
{
    mat4_t m = {{{0}}};
    m.m[0][0] = 1.0f / (aspect * tanf(fov / 2));
    m.m[1][1] = 1.0f / tanf(fov / 2);
    m.m[2][2] = zfar / (zfar - znear);
    m.m[2][3] = (-zfar * znear) / (zfar - znear);
    m.m[3][2] = 1.0;
    return m;
}


function vec4_t mat4_mul_vec4_project(mat4_t mat_proj, vec4_t v)
{
    vec4_t result = mat4_mul_vec4(mat_proj, v);

    // perspective divide with original z-value that is now stored in w
    if (result.w != 0.0f)
    {
        result.x /= result.w;
        result.y /= result.w;
        result.z /= result.w;
    }
    return result;
}

function mat4_t mat4_look_at(vec3_t eye, vec3_t target, vec3_t up)
{
    vec3_t z = vec3_normalize(vec3_sub(target, eye));
    vec3_t x = vec3_normalize(vec3_cross(up, z));
    vec3_t y = vec3_cross(z, x);

    mat4_t view_matrix = {
        {
            {x.x, x.y, x.z, -vec3_dot(x, eye)},
            {y.x, y.y, y.z, -vec3_dot(y, eye)},
            {z.x, z.y, z.z, -vec3_dot(z, eye)},
            {0, 0, 0, 1}
        }
    };

    return view_matrix;
}


function U32 light_apply_intensity(U32 orginial_color, F32 percentage_factor)
{
    if (percentage_factor < 0) percentage_factor = 0;
    else if (percentage_factor > 1) percentage_factor = 1;

    U32 a = (orginial_color & 0xFF000000);
    U32 r = (orginial_color & 0x00FF0000) * percentage_factor;
    U32 g = (orginial_color & 0x0000FF00) * percentage_factor;
    U32 b = (orginial_color & 0x000000FF) * percentage_factor;

    U32 new_color = a | r & 0x00FF0000 | g & 0x0000FF00 | b & 0x000000FF;
    return new_color;
}
