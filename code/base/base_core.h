//
// Created by Karim on 2024-11-12.
//

#ifndef BASE_CORE_H
#define BASE_CORE_H

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "stdio.h"
#include "stdbool.h"
#include "math.h"
#include <assert.h>
#include <stdalign.h>


//~ karim: Base-Types

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


#endif //BASE_CORE_H
