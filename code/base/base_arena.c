// base_arena.c
#include <assert.h>

// Thread-local storage
_Thread_local static arena_t scratch_arenas[SCRATCH_ARENA_COUNT];
_Thread_local static B32 scratch_initialized = 0;

// Internal helpers
static U64 align_up(U64 value, U64 alignment) {
    return (value + alignment - 1) & ~(alignment - 1);
}

arena_t arena_alloc(U64 size) {
    const U64 page_size = get_page_size();
    arena_t arena = {0};

    arena.reserved_size = align_up(size, page_size);
    arena.memory = os_reserve(arena.reserved_size);
    assert(arena.memory && "Failed to reserve virtual memory");

    return arena;
}

void* arena_push(arena_t* arena, U64 size, U64 align) {
    // Validate alignment requirements
    assert(align != 0 && "Alignment must be > 0");
    assert((align & (align - 1)) == 0 && "Alignment must be power of two");

    const U64 base = (U64)arena->memory;
    const U64 current = base + arena->pos;
    const U64 aligned = align_up(current, align);
    const U64 offset = aligned - base;
    const U64 required_size = offset + size;

    // Commit memory if needed
    if (required_size > arena->committed_size) {
        const U64 page_size = get_page_size();
        const U64 commit_needed = required_size - arena->committed_size;
        const U64 commit_size = align_up(commit_needed, page_size);
        const U64 commit_offset = arena->committed_size;

        // Check reserve capacity
        if ((commit_offset + commit_size) > arena->reserved_size) {
            assert(0 && "Arena out of reserved memory");
            return NULL;
        }

        // Commit physical memory
        void* commit_addr = (U8*)arena->memory + commit_offset;
        if (!os_commit(commit_addr, commit_size)) {
            assert(0 && "Failed to commit memory");
            return NULL;
        }

        arena->committed_size += commit_size;
    }

    arena->pos = required_size;
    return (void*)aligned;
}

void arena_reset(arena_t* arena) {
    arena->pos = 0;
}

void arena_clear(arena_t* arena) {
    if (arena->committed_size > 0) {
        os_decommit(arena->memory, arena->committed_size);
        arena->committed_size = 0;
    }
    arena->pos = 0;
}

void arena_free(arena_t* arena) {
    if (arena->memory) {
        arena_clear(arena);
        os_release(arena->memory, arena->reserved_size);
        arena->memory = 0;
        arena->reserved_size = 0;
    }
}

temp_t temp_begin(arena_t* arena) {
    return (temp_t){ .arena = arena, .pos = arena->pos };
}

void temp_end(temp_t temp) {
    temp.arena->pos = temp.pos;
}

arena_t* scratch_begin(void) {
    // Lazy initialization
    if (!scratch_initialized) {
        for (U32 i = 0; i < SCRATCH_ARENA_COUNT; i++) {
            scratch_arenas[i] = arena_alloc(ARENA_RESERVE_SIZE);
            scratch_arenas[i].in_use = 0;
        }
        scratch_initialized = 1;
    }

    // Find first available arena
    for (U32 i = 0; i < SCRATCH_ARENA_COUNT; i++) {
        if (!scratch_arenas[i].in_use) {
            scratch_arenas[i].in_use = 1;
            arena_reset(&scratch_arenas[i]);
            return &scratch_arenas[i];
        }
    }

    assert(0 && "All scratch arenas in use");
    return NULL;
}

void scratch_end(arena_t* arena) {
    arena->in_use = 0;
    arena_reset(arena);
}