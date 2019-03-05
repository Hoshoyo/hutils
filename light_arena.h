#ifndef H_LIGHT_ARENA
#define H_LIGHT_ARENA

/*
    Author: Pedro Sassen Veiga
    The MIT License

    This library is C89 compatible

    ----------------------------------------------------------------------------------

    define LIGHT_ARENA_NO_CRT if you don't want the c runtime library included
    if that is defined, you must provide implementations for the following functions:

    void* calloc(size_t num, size_t size)
    void  free(void* block)

    ----------------------------------------------------------------------------------

    Usage:
    An example usage of creating, allocating and freeing an arena:

    #include "light_arena.h"

    int main(int argc, char** argv)
    {
        Light_Arena* arena = arena_create(512);

        void* m1 = arena_alloc(arena, 64);
        void* m2 = arena_alloc(arena, 84);

        arena_free(arena);

        return 0;
    }

    The arena will grow and is not limited by the block size, new blocks will be allocated
    incrementing by the initial size each time.
*/

#if !defined(LIGHT_ARENA_NO_CRT)
#include <stdlib.h>
#endif

#define LIGHT_ARENA_API extern

typedef struct Light_Arena_t{
    size_t capacity;
    void*  ptr;
    struct Light_Arena_t* next;
    struct Light_Arena_t* last;
} Light_Arena;

LIGHT_ARENA_API Light_Arena* arena_create(size_t size);
LIGHT_ARENA_API void* arena_alloc(Light_Arena* arena, size_t size_bytes);
LIGHT_ARENA_API void  arena_free(Light_Arena* arena);
LIGHT_ARENA_API void  arena_clear(Light_Arena* arena);

#if defined(LIGHT_ARENA_IMPLEMENT)
/* creates an arena with 'size' number of bytes of block, meaning it will take 'size' bytes
   until a new allocation happens, since an allocation of 'size' is performed, although the
   arena will grow dynamically with new blocks of 'size' bytes each growth. */
LIGHT_ARENA_API Light_Arena* 
arena_create(size_t size) {
    Light_Arena* base = (Light_Arena*)calloc(1, size + sizeof(Light_Arena));
	base->capacity = size;
	base->ptr = (void*)(base + 1);
	base->last = base;
	base->next = 0;
    return base;
}

/* allocates memory in the arena and may cause a growth in the size of it. Allocation works
   just like 'calloc', meaning the memory will be zeroed. */
LIGHT_ARENA_API void*
arena_alloc(Light_Arena* arena, size_t size_bytes) {
#define arena_size(A) ((char*)((A)->ptr) - (char*)(A) + sizeof(Light_Arena))
    void* result;
    if (size_bytes + arena_size(arena->last) > arena->last->capacity) {
        Light_Arena* narena = arena_create(arena->capacity);
        arena->last->next = narena;
        arena->last = narena;
		arena->ptr = narena->ptr;
    }
#undef arena_size
    result = arena->last->ptr;
    arena->last->ptr = (char*)arena->last->ptr + size_bytes;
    return result;
}

/* frees all arena content making its pointer invalid. */
LIGHT_ARENA_API void
arena_free(Light_Arena* arena) {
	Light_Arena* aux = arena;
	while (aux) {
		Light_Arena* next = aux->next;
		free(aux);
		aux = next;
	}
}

/* clears all the memory in the arena, not freeing it */
LIGHT_ARENA_API void
arena_clear(Light_Arena* arena) {
	Light_Arena* aux = arena;
	while (aux) {
		Light_Arena* next = aux->next;
        aux->ptr = aux + 1;
		aux = next;
	}
}

#endif /* H_LIGHT_ARENA */
#endif