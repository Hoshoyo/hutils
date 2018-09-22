#ifndef HO_MEMORY_ARENA
#define HO_MEMORY_ARENA

/*
    Author: Pedro Sassen Veiga
    This library is C89 compatible
*/

#if !defined(LIGHT_ARENA_NO_CRT)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

typedef struct Light_Arena_t{
#ifdef LIGHT_ARRAY_64BIT
    unsigned long long capacity;
#else
    unsigned int capacity;
#endif
    void* ptr;
    struct Light_Arena_t* next;
    struct Light_Arena_t* last;
} Light_Arena;

#define LIGHT_ARENA_MIN(x, y) (y ^ ((x ^ y) & -(x < y)))
#define LIGHT_ARENA_MAX(x, y) (x ^ ((x ^ y) & -(x < y)))

#define arena_ptr(A) (A)->ptr
#define arena_capacity(A) (A)->capacity
#define arena_size(A) ((char*)((A)->ptr) - (char*)(A) + sizeof(Light_Arena))

static Light_Arena* 
arena_create(size_t size) {
    Light_Arena* res = (Light_Arena*)calloc(1, size + sizeof(Light_Arena));
    res->capacity = size;
    res->ptr = (void*)(res + 1);
    res->next = 0;
    return res;
}

void*
arena_alloc(Light_Arena* arena, size_t size_bytes) {
    void* result;
    if (size_bytes + arena_size(arena) > arena_capacity(arena)) {
        Light_Arena* narena = arena_create(arena_capacity(arena));
        arena->next = narena;
    } else {
        result = arena->ptr;
        arena->ptr = (char*)arena->ptr + size_bytes;
    }
    return result;
}

static void
arena_free(Light_Arena* arena) {
    if(!arena) return;
    arena_free(arena->next);
    free(arena);
}

//#define arena_clear(A) arena_ptr(A) = (void*)((Arena_Base*)(arena_base(A)) + 1)

#endif
