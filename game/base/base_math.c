//
// Created by Karim on 2024-11-12.
//

vec3_t vec3_rotate_x(vec3_t v, F32 angle)
{
    vec3_t rotated_vector = {
        .x = v.x,
        .y = v.y * cosf(angle) - v.z * sinf(angle),
        .z = v.y * sinf(angle) + v.z * cosf(angle)
};

    return rotated_vector;
}
vec3_t vec3_rotate_y(vec3_t v, F32 angle)
{
    vec3_t rotated_vector = {
        .x = v.x * cosf(angle) - v.z * sinf(angle),
        .y = v.y,
        .z = v.x * sinf(angle) + v.z * cosf(angle)
};

    return rotated_vector;
}
vec3_t vec3_rotate_z(vec3_t v, F32 angle)
{
    vec3_t rotated_vector = {
        .x = v.x * cosf(angle) - v.y * sinf(angle),
        .y = v.x * sinf(angle) + v.y * cosf(angle),
        .z = v.z
};

    return rotated_vector;
}

// Projection
vec2_t project(const vec3_t point)
{
    const vec2_t projected_point = {
        point.x / point.z,
        point.y / point.z
    };
    return projected_point;
}
