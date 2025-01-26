//
// Created by Karim on 2024-11-13.
//

#ifndef BASE_MEMORY_H
#define BASE_MEMORY_H

typedef struct arena_t arena_t;

struct arena_t
{
    U8* memory;
    U64 reserved_size;
    U64 committed_size;
    U64 pos;
    B32 in_use;
};

typedef struct temp_t temp_t;

struct temp_t
{
    arena_t* arena;
    U64 pos;
};

function arena_t arena_alloc(U64 size);
function void* arena_push(arena_t* arena, U64 size, U64 align);
function void arena_reset(arena_t* arena);
function void arena_clear(arena_t* arena);
function void arena_free(arena_t* arena);

function temp_t temp_begin(arena_t* arena);
function void temp_end(temp_t temp);

function void initialize_scratch_arena();
function arena_t* scratch_begin();
function void scratch_end(arena_t* arena);

// System page size (4KB typical)
#define PAGE_SIZE 4096
#define SCRATCH_ARENA_COUNT 4
#define ARENA_RESERVE_SIZE (128 * 1024 * 1024) // 128MB per arena

#define push_array(arena, type, count) (type *)arena_push((arena), sizeof(type)*(count), alignof(type))


#endif //BASE_MEMORY_H
