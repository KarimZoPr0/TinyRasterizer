//
// Created by Abdik on 2025-01-25.
//

#ifndef CAMERA_H
#define CAMERA_H

typedef struct camera_t camera_t;
struct camera_t
{
    vec3_t position;
    vec3_t direction;
    vec3_t forward_velocity;
    F32 yaw;
};

extern camera_t camera;

#endif //CAMERA_H
