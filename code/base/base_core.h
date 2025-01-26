//
// Created by Karim on 2024-11-12.
//

#ifndef BASE_CORE_H
#define BASE_CORE_H

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "math.h"
#include <assert.h>
#include <stdalign.h>


//~ karim: base-types

#define local_persist static
#define function static
#define global static

typedef int8_t   S8;
typedef int16_t  S16;
typedef int32_t  S32;
typedef int64_t  S64;
typedef uint8_t  U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;
typedef S8       B8;
typedef S16      B16;
typedef S32      B32;
typedef S64      B64;
typedef float    F32;
typedef double   F64;

#define Bytes(n)      (n)
#define Kilobytes(n)  (n << 10)
#define Megabytes(n)  (n << 20)
#define Gigabytes(n)  (((U64)n) << 30)
#define Terabytes(n)  (((U64)n) << 40)

#define Swap(T,a,b) do{T t__ = a; a = b; b = t__;}while(0)
#define ARGB(a, r, g, b) ((U32)((a << 24) | (r << 16) | (g << 8) | (b)))

////////////////////////////////
//~ karim: Linked List Building Macros

//- karim: linked list macro helpers
#define CheckNil(nil,p) ((p) == 0 || (p) == nil)
#define SetNil(nil,p) ((p) = nil)

//- karim: doubly-linked-lists
#define DLLInsert_NPZ(nil,f,l,p,n,next,prev) (CheckNil(nil,f) ? \
((f) = (l) = (n), SetNil(nil,(n)->next), SetNil(nil,(n)->prev)) :\
CheckNil(nil,p) ? \
((n)->next = (f), (f)->prev = (n), (f) = (n), SetNil(nil,(n)->prev)) :\
((p)==(l)) ? \
((l)->next = (n), (n)->prev = (l), (l) = (n), SetNil(nil, (n)->next)) :\
(((!CheckNil(nil,p) && CheckNil(nil,(p)->next)) ? (0) : ((p)->next->prev = (n))), ((n)->next = (p)->next), ((p)->next = (n)), ((n)->prev = (p))))
#define DLLPushBack_NPZ(nil,f,l,n,next,prev) DLLInsert_NPZ(nil,f,l,l,n,next,prev)
#define DLLPushFront_NPZ(nil,f,l,n,next,prev) DLLInsert_NPZ(nil,l,f,f,n,prev,next)
#define DLLRemove_NPZ(nil,f,l,n,next,prev) (((n) == (f) ? (f) = (n)->next : (0)),\
((n) == (l) ? (l) = (l)->prev : (0)),\
(CheckNil(nil,(n)->prev) ? (0) :\
((n)->prev->next = (n)->next)),\
(CheckNil(nil,(n)->next) ? (0) :\
((n)->next->prev = (n)->prev)))

//- karim: singly-linked, doubly-headed lists (queues)
#define SLLQueuePush_NZ(nil,f,l,n,next) (CheckNil(nil,f)?\
((f)=(l)=(n),SetNil(nil,(n)->next)):\
((l)->next=(n),(l)=(n),SetNil(nil,(n)->next)))
#define SLLQueuePushFront_NZ(nil,f,l,n,next) (CheckNil(nil,f)?\
((f)=(l)=(n),SetNil(nil,(n)->next)):\
((n)->next=(f),(f)=(n)))
#define SLLQueuePop_NZ(nil,f,l,next) ((f)==(l)?\
(SetNil(nil,f),SetNil(nil,l)):\
((f)=(f)->next))

#define SLLQueueMoveHead_NZ(nil, f1, l1, f2, next) \
(CheckNil(nil, f1) && CheckNil(nil, l1) ? \
((l1)->next = (f2), SetNil(nil, f2), (f2) = (f1)) : \
SetNil(nil, f2))



//- karim: singly-linked, singly-headed lists (stacks)
#define SLLStackPush_N(f,n,next) ((n)->next=(f), (f)=(n))
#define SLLStackPop_N(f,next) ((f)=(f)->next)

//- karim: doubly-linked-list helpers
#define DLLInsert_NP(f,l,p,n,next,prev) DLLInsert_NPZ(0,f,l,p,n,next,prev)
#define DLLPushBack_NP(f,l,n,next,prev) DLLPushBack_NPZ(0,f,l,n,next,prev)
#define DLLPushFront_NP(f,l,n,next,prev) DLLPushFront_NPZ(0,f,l,n,next,prev)
#define DLLRemove_NP(f,l,n,next,prev) DLLRemove_NPZ(0,f,l,n,next,prev)
#define DLLInsert(f,l,p,n) DLLInsert_NPZ(0,f,l,p,n,next,prev)
#define DLLPushBack(f,l,n) DLLPushBack_NPZ(0,f,l,n,next,prev)
#define DLLPushFront(f,l,n) DLLPushFront_NPZ(0,f,l,n,next,prev)
#define DLLRemove(f,l,n) DLLRemove_NPZ(0,f,l,n,next,prev)

//- karim: singly-linked, doubly-headed list helpers
#define SLLQueuePush_N(f,l,n,next) SLLQueuePush_NZ(0,f,l,n,next)
#define SLLQueuePushFront_N(f,l,n,next) SLLQueuePushFront_NZ(0,f,l,n,next)
#define SLLQueuePop_N(f,l,next) SLLQueuePop_NZ(0,f,l,next)
#define SLLQueuePush(f,l,n) SLLQueuePush_NZ(0,f,l,n,next)
#define SLLQueuePushFront(f,l,n) SLLQueuePushFront_NZ(0,f,l,n,next)
#define SLLQueuePop(f,l) SLLQueuePop_NZ(0,f,l,next)
#define SLLQueueMoveHead(f1, l1, f2) \
SLLQueueMoveHead_NZ(0, f1, l1, f2, next)


//- karim: singly-linked, singly-headed list helpers
#define SLLStackPush(f,n) SLLStackPush_N(f,n,next)
#define SLLStackPop(f) SLLStackPop_N(f,next)

#define MemoryCopy(dest, src, n) memcpy(dest, src, n)

// Helper macros for concatenation
#define CONCATENATE(arg1, arg2) CONCATENATE1(arg1, arg2)
#define CONCATENATE1(arg1, arg2) CONCATENATE2(arg1, arg2)
#define CONCATENATE2(arg1, arg2) arg1##arg2

// Helper macros for counting arguments
#define COUNT_VARARGS(...) COUNT_VARARGS_IMPL(__VA_ARGS__, 10,9,8,7,6,5,4,3,2,1,0)
#define COUNT_VARARGS_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,N,...) N

// Helper macros for iterating over arguments
#define FOR_EACH(macro, obj, ...) \
CONCATENATE(FOR_EACH_, COUNT_VARARGS(__VA_ARGS__))(macro, obj, __VA_ARGS__)

#define FOR_EACH_1(macro, obj, x) macro(obj, x)
#define FOR_EACH_2(macro, obj, x, ...) macro(obj, x) FOR_EACH_1(macro, obj, __VA_ARGS__)
#define FOR_EACH_3(macro, obj, x, ...) macro(obj, x) FOR_EACH_2(macro, obj, __VA_ARGS__)
#define FOR_EACH_4(macro, obj, x, ...) macro(obj, x) FOR_EACH_3(macro, obj, __VA_ARGS__)
#define FOR_EACH_5(macro, obj, x, ...) macro(obj, x) FOR_EACH_4(macro, obj, __VA_ARGS__)
#define FOR_EACH_6(macro, obj, x, ...) macro(obj, x) FOR_EACH_5(macro, obj, __VA_ARGS__)
#define FOR_EACH_7(macro, obj, x, ...) macro(obj, x) FOR_EACH_6(macro, obj, __VA_ARGS__)
#define FOR_EACH_8(macro, obj, x, ...) macro(obj, x) FOR_EACH_7(macro, obj, __VA_ARGS__)
#define FOR_EACH_9(macro, obj, x, ...) macro(obj, x) FOR_EACH_8(macro, obj, __VA_ARGS__)
#define FOR_EACH_10(macro, obj, x, ...) macro(obj, x) FOR_EACH_9(macro, obj, __VA_ARGS__)

// Macro to zero a single field for non-pointer objects (using .)
#define ZERO_ONE_FIELD(obj, field) (obj).field = 0;

// Macro to zero a single field for pointer objects (using ->)
#define ZERO_ONE_FIELD_PTR(obj, field) (obj)->field = 0;

// Main ZeroFieldsIn macro for non-pointer objects
#define ZeroFieldsIn(obj, ...) \
do { \
FOR_EACH(ZERO_ONE_FIELD, obj, __VA_ARGS__) \
} while (0)

// Main ZeroFieldsIn macro for pointer objects
#define ZeroFieldsInPtr(obj, ...) \
do { \
FOR_EACH(ZERO_ONE_FIELD_PTR, obj, __VA_ARGS__) \
} while (0)


#endif //BASE_CORE_H
