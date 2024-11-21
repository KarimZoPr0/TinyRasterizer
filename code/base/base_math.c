//
// Created by Karim on 2024-11-12.
//
// Vector 2D functions
function float vec2_length(vec2_t v)
{
    return sqrtf(v.x * v.x + v.y * v.y);
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

function float vec2_dot(vec2_t a, vec2_t b)
{
    return a.x * b.x + a.y * b.y;
}


// Vector 3D functions
function float vec3_length(vec3_t v)
{
    return sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
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

function float vec3_dot(vec3_t a, vec3_t b)
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

// Projection
function vec2_t project(const vec3_t point)
{
    const vec2_t projected_point = {
        point.x / point.z,
        point.y / point.z
    };
    return projected_point;
}
