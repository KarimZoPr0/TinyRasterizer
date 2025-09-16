#ifndef BASE_ARENA_H
#define BASE_ARENA_H

#define ARENA_HEADER_SIZE          128ULL
#define DEFAULT_ARENA_RESERVE_SIZE MB(64)
#define DEFAULT_COMMIT_INCREMENT   KB(64)

typedef struct arena_t arena_t;
typedef struct temp_t  temp_t;

struct arena_t {
    arena_t* prev;           /* Pointer to the previous block in the chain */
    arena_t* current;        /* Pointer to the block used for new allocations (top of chain) */
    U64 reserve_size;        /* Total reserved size of this block */
    U64 commit_increment;    /* How many bytes to commit at a time */
    U64 base_offset;         /* Absolute offset of this block in the overall arena */
    U64 pos;                 /* Current allocation offset within this block (after header) */
    U64 committed;           /* Amount of memory committed in this block (>= pos) */
};

struct temp_t {
    arena_t* arena;
    U64 pos;
};

arena_t* arena_create(void);
arena_t* arena_create_custom(U64 reserve_size, U64 commit_increment);

U64 arena_get_pos(arena_t* arena);
void* arena_push(arena_t* arena, U64 size, U64 align);

void arena_pop_to(arena_t* arena, U64 target_pos);
void arena_clear(arena_t* arena);
void arena_free(arena_t* arena);

temp_t temp_begin(arena_t* arena);
void temp_end(temp_t temp);

temp_t scratch_begin(arena_t** conflicts, U64 conflict_count);
#define scratch_end(temp) temp_end(temp)

#define push_array_no_zero_aligned(arena, type, count, align) \
    ((type *) arena_push((arena), sizeof(type) * (count), (align)))

#define push_array_aligned(arena, type, count, align) \
    ((type *) MemoryZero(push_array_no_zero_aligned((arena), type, (count), (align)), sizeof(type) * (count)))

#define push_array_no_zero(arena, type, count) \
    push_array_no_zero_aligned((arena), type, (count), MAX(8, (unsigned)_Alignof(type)))

#define push_array(arena, type, count) \
    push_array_aligned((arena), type, (count), MAX(8, (unsigned)_Alignof(type)))

_Thread_local arena_t *scratch_arenas[2];

#ifdef __cplusplus
}
#endif

#endif // BASE_ARENA_H
