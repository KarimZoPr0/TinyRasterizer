//
// Created by Karim on 2024-11-13.
//

#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

typedef struct Arena {
    U8 *memory;
    U64 reserved_size;
    U64 committed_size;
    U64 pos;
    B32 in_use;
} Arena;

typedef struct Temp {
    Arena *arena;
    U64 pos;
} Temp;

Arena arena_alloc(U64 size);
void *arena_push(Arena *arena, U64 size, U64 align);
void arena_reset(Arena *arena);
void arena_clear(Arena *arena);
void arena_free(Arena *arena);

Temp temp_begin(Arena *arena);
void temp_end(Temp temp);

void initialize_scratch_arena();
Arena* scratch_begin();
void scratch_end(Arena *arena);

#define SCRATCH_ARENA_COUNT 2
#define SCRATCH_ARENA_SIZE Gigabytes(2)

#define push_array(arena, type, count) (type *)arena_push((arena), sizeof(type)*(count), alignof(type))

#endif //BASE_MEMORY_H
