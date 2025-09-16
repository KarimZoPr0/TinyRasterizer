#include "base_arena.h"
#include <assert.h>

/*===========================================================================
  Internal: Alignment Helper
===========================================================================*/
function U64 align_up(U64 value, U64 alignment)
{
    return (value + alignment - 1) & ~(alignment - 1);
}

/*===========================================================================
  Internal: Low-Level Block Allocation

  Allocates one arena block. Only the header (ARENA_HEADER_SIZE) is committed.
===========================================================================*/
function arena_t* arena_alloc_block(U64 reserve_size, U64 commit_increment)
{
    const U64 page_size = get_page_size();
    reserve_size = align_up(reserve_size, page_size);
    commit_increment = align_up(commit_increment, page_size);

    /* Reserve virtual memory. */
    void* block = os_reserve(reserve_size);
    assert(block && "os_reserve failed for arena block");

    /* Commit only the header. */
    int commit_ok = os_commit(block, ARENA_HEADER_SIZE);
    assert(commit_ok && "os_commit failed for arena header");

    arena_t* arena = (arena_t*)block;
    arena->prev = NULL;
    arena->current = arena; /* Initially, this block is the current one. */
    arena->reserve_size = reserve_size;
    arena->commit_increment = commit_increment;
    arena->base_offset = 0; /* For the first block, base offset is zero. */
    arena->pos = ARENA_HEADER_SIZE; /* Allocations start right after the header. */
    arena->committed = ARENA_HEADER_SIZE; /* Only the header is committed initially. */
    return arena;
}

/*===========================================================================
  Arena Creation Functions
===========================================================================*/
arena_t* arena_create(void)
{
    return arena_alloc_block(DEFAULT_ARENA_RESERVE_SIZE, DEFAULT_COMMIT_INCREMENT);
}

arena_t* arena_create_custom(U64 reserve_size, U64 commit_increment)
{
    return arena_alloc_block(reserve_size, commit_increment);
}

/*===========================================================================
  Arena Allocation Functions
===========================================================================*/
U64 arena_get_pos(arena_t* arena)
{
    return arena->current->base_offset + arena->current->pos;
}

void* arena_push(arena_t* arena, U64 size, U64 align)
{
    assert(align != 0 && (align & (align - 1)) == 0 &&
        "Alignment must be a nonzero power of 2");
    arena_t* current = arena->current;
    U64 base_addr = (U64)current; /* Starting address of the current block. */

    /* Compute the aligned address within the current block. */
    U64 allocation_start = base_addr + current->pos;
    U64 aligned_addr = align_up(allocation_start, align);
    U64 new_pos = (aligned_addr - base_addr) + size; /* New offset within current block. */

    /* If the allocation would exceed the reserved region, chain a new block. */
    if (new_pos > current->reserve_size)
    {
        /* Decide on new block size. Use current block's reserve size by default,
           but if the requested size is huge, enlarge the new block. */
        U64 new_block_reserve = current->reserve_size;
        if (size + ARENA_HEADER_SIZE > new_block_reserve)
        {
            new_block_reserve = align_up(size + ARENA_HEADER_SIZE, align);
        }
        arena_t* new_block = arena_alloc_block(new_block_reserve, current->commit_increment);
        new_block->base_offset = current->base_offset + current->reserve_size;
        new_block->prev = current;
        new_block->current = new_block; /* New block becomes the current block. */
        arena->current = new_block;
        current = new_block;

        /* Recompute allocation pointers in the new block. */
        base_addr = (U64)current;
        current->pos = ARENA_HEADER_SIZE; /* Start after header. */
        allocation_start = base_addr + current->pos;
        aligned_addr = align_up(allocation_start, align);
        new_pos = (aligned_addr - base_addr) + size;
    }

    /* Commit additional memory if needed. */
    if (new_pos > current->committed)
    {
        U64 commit_target = align_up(new_pos, current->commit_increment);
        if (commit_target > current->reserve_size)
            commit_target = current->reserve_size; /* Clamp to reserved size. */
        void* commit_addr = (U8*)current + current->committed;
        int commit_ok = os_commit(commit_addr, commit_target - current->committed);
        assert(commit_ok && "os_commit failed during arena_push");
        current->committed = commit_target;
    }

    /* Update allocation pointer and return the allocated memory. */
    current->pos = new_pos;
    return (void*)aligned_addr;
}

/*===========================================================================
  Arena Reset / Pop / Free Functions
===========================================================================*/
void arena_pop_to(arena_t* arena, U64 target_pos)
{
    /* Never pop below the header of the base block. */
    if (target_pos < ARENA_HEADER_SIZE)
        target_pos = ARENA_HEADER_SIZE;

    arena_t* current = arena->current;
    /* If the target is in an earlier block, release blocks until we reach it. */
    while (current->base_offset > target_pos && current->prev != NULL)
    {
        arena_t* prev = current->prev;
        os_release((void*)current, current->reserve_size);
        current = prev;
        arena->current = current;
    }
    U64 new_block_pos = target_pos - current->base_offset;
    assert(new_block_pos <= current->pos);
    current->pos = new_block_pos;
}

void arena_clear(arena_t* arena)
{
    arena_pop_to(arena, ARENA_HEADER_SIZE);
}

void arena_free(arena_t* arena)
{
    arena_t* current = arena->current;
    while (current != NULL)
    {
        arena_t* prev = current->prev;
        os_release((void*)current, current->reserve_size);
        current = prev;
    }
}

/*===========================================================================
  Temporary Allocation Scopes
===========================================================================*/
temp_t temp_begin(arena_t* arena)
{
    temp_t temp;
    temp.arena = arena;
    temp.pos = arena_get_pos(arena);
    return temp;
}

void temp_end(temp_t temp)
{
    arena_pop_to(temp.arena, temp.pos);
}

/*===========================================================================
  Scratch Arena Management (Using Temporary Scopes)
===========================================================================*/

temp_t scratch_begin(arena_t** conflicts, U64 conflict_count)
{
    temp_t temp = {0}; // Initialize temp state to zero.
    for (U64 i = 0; i < ArrayCount(scratch_arenas); i++)
    {
        // Create the arena on–demand.
        if (!scratch_arenas[i])
        {
            scratch_arenas[i] = arena_create();
        }
        // Check for conflict.
        B32 is_conflicting = 0;
        for (U64 j = 0; j < conflict_count; j++)
        {
            if (conflicts[j] == scratch_arenas[i])
            {
                is_conflicting = 1;
                break;
            }
        }
        // Use this arena if it is not conflicting.
        if (!is_conflicting)
        {
            temp.arena = scratch_arenas[i];
            temp.pos = scratch_arenas[i]->pos;
            break;
        }
    }
    return temp;
}
