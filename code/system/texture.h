//
// Created by Abdik on 2025-03-16.
//

#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct tex2_t tex2_t;
struct tex2_t
{
    F32 u;
    F32 v;
};

extern int texture_width;
extern int texture_height;
extern const U8 REDBRICK_TEXTURE[];
extern U32 *mesh_texture;

#endif //TEXTURE_H
