//
// Created by Abdik on 2024-11-13.
//
_Thread_local  Arena scratch_arenas[SCRATCH_ARENA_COUNT];

Arena arena_alloc(U64 size)
{
    Arena arena = {0};
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

void* arena_push(Arena *arena, U64 size, U64 align)
{
    uintptr_t current = (uintptr_t)(arena->memory + arena->pos);
    uintptr_t aligned = (current + (align - 1)) & ~(align - 1);
    U64 offset = aligned - (uintptr_t)arena->memory;
    U64 required_size = offset + size;

    if (required_size > arena->committed_size)
    {
        // Commit memory in 1MB blocks
        U64 page_size = Megabytes(1);
        U64 commit_size = ((required_size - arena->committed_size) + (page_size -1)) & ~(page_size -1);
        if(!os_commit(arena->memory + arena->committed_size, commit_size))
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

void arena_reset(Arena* arena)
{
    arena->pos = 0;
}

void arena_clear(Arena* arena)
{
    arena_reset(arena);
    if (arena->committed_size > 0)
    {
        os_decommit(arena->memory, 0);
        arena->committed_size = 0;
    }
}

void arena_free(Arena* arena)
{
    if (arena->memory)
    {
        os_release(arena->memory, 0);
        memset(arena, 0, sizeof(Arena));
    }
}

Temp temp_begin(Arena* arena)
{
    Temp temp;
    temp.arena = arena;
    temp.pos = arena->pos;
    return temp;
}

void temp_end(Temp temp)
{
    temp.arena->pos = temp.pos;
}


void initialize_scratch_arena() {
    for (U32 i = 0; i < SCRATCH_ARENA_COUNT; ++i) {
        scratch_arenas[i] = arena_alloc(SCRATCH_ARENA_SIZE);
        scratch_arenas[i].in_use = false;
    }
}

Arena* scratch_begin() {
    for (U32 i = 0; i < SCRATCH_ARENA_COUNT; ++i) {
        if (!scratch_arenas[i].in_use) {
            scratch_arenas[i].in_use = true;
            return &scratch_arenas[i];
        }
    }
    assert(0 && "No available scratch arenas");
    return NULL;
}

void scratch_end(Arena *arena) {
    assert(arena->in_use && "Attempting to release a scratch arena that's not in use");
    arena_reset(arena);
    arena->in_use = false;
}
