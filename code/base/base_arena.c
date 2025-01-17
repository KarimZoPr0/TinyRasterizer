//
// Created by Karim on 2024-11-13.
//

_Thread_local arena_t scratch_arenas[SCRATCH_ARENA_COUNT];

function arena_t arena_alloc(U64 size)
{
    arena_t arena = {0};
    arena.reserved_size = size;
    arena.memory = os_reserve(size);
    if (!arena.memory)
    {
        assert(0 && "Failed to reserve virtual memory");
    }
    arena.pos = 0;
    arena.committed_size = 0;
    return arena;
}

function void* arena_push(arena_t* arena, U64 size, U64 align)
{
    uintptr_t current = (uintptr_t)(arena->memory + arena->pos);
    uintptr_t aligned = (current + (align - 1)) & ~(align - 1);
    U64 offset = aligned - (uintptr_t)arena->memory;
    U64 required_size = offset + size;

    if (required_size > arena->committed_size)
    {
        // Commit memory in 1MB blocks
        U64 page_size = Megabytes(1);
        U64 commit_size = ((required_size - arena->committed_size) + (page_size - 1)) & ~(page_size - 1);
        if (!os_commit(arena->memory + arena->committed_size, commit_size))
        {
            assert(0 && "Failed to commit memory");
            return NULL;
        }
        arena->committed_size += commit_size;
    }

    if (required_size > arena->reserved_size)
    {
        assert(0 && "Arena out of reserved memory");
        return NULL;
    }

    arena->pos = required_size;
    return (void*)aligned;
}

function void arena_reset(arena_t* arena)
{
    arena->pos = 0;
}

function void arena_clear(arena_t* arena)
{
    arena_reset(arena);
    if (arena->committed_size > 0)
    {
        os_decommit(arena->memory, 0);
        arena->committed_size = 0;
    }
}

function void arena_free(arena_t* arena)
{
    if (arena->memory)
    {
        os_release(arena->memory, 0);
        memset(arena, 0, sizeof(arena_t));
    }
}

temp_t temp_begin(arena_t* arena)
{
    temp_t temp;
    temp.arena = arena;
    temp.pos = arena->pos;
    return temp;
}

function void temp_end(temp_t temp)
{
    temp.arena->pos = temp.pos;
}

function void initialize_scratch_arena()
{
    static B32 initialized = 0;
    if (!initialized)
    {
        for (U32 i = 0; i < SCRATCH_ARENA_COUNT; ++i)
        {
            scratch_arenas[i] = arena_alloc(Gigabytes(2));
            scratch_arenas[i].in_use = 0;
            initialized = 1;
        }
    }

}

function arena_t *scratch_begin()
{
    local_persist B32 initialized = 0;
    if (!initialized)
    {
        for (U32 i = 0; i < SCRATCH_ARENA_COUNT; ++i)
        {
            scratch_arenas[i] = arena_alloc(Gigabytes(2));
            scratch_arenas[i].in_use = 0;
            initialized = 1;
        }
    }

    arena_t *arena = {0};
    for (U32 i = 0; i < SCRATCH_ARENA_COUNT; ++i)
    {
        if (!scratch_arenas[i].in_use)
        {
            scratch_arenas[i].in_use = 1;
            arena = &scratch_arenas[i];
            break;
        }
    }
    return arena;
}

function void scratch_end(arena_t *arena)
{
    arena_reset(arena);
    arena->in_use = 0;
}
