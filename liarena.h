#ifndef H_LIGHT_ARENA
#define H_LIGHT_ARENA

/*
    Author: Pedro Sassen Veiga
    The MIT License

    This library is C89 compatible

    ----------------------------------------------------------------------------------

    Usage:
    An example usage of creating, allocating and freeing an arena:

    #define LIGHT_ARENA_IMPLEMENT
    #include "liarena.h"

    int main()
    {
        // Create an arena with default LIGHT_ARENA_MAX_RESERVED_VIRTUAL_SPACE_GB Gigabytes of max space.
        Light_Arena* arena = liarena_create();

        // Allocates 128 bytes aligned to 8 bytes by default.
        void* mem_aligned8 = liarena_alloc(arena, 128);

        // Allocates 1024 bytes aligned to 32 bytes.
        void* mem_aligned32 = liarena_alloc_aligned(arena, 1024, 32);

        // Allocates 64 bytes unaligned (aligned to 0 bytes).
        void* mem_unaligned = liarena_alloc_unaligned(arena, 64);

        // Resets all the allocations in the arena, but doesn't release
        // the memory back to the OS.
        liarena_clear(arena);

        // Releases all the extra memory not needed currently to the OS.
        liarena_trim(arena);

        // Free's the arena completely, after this point the arena is invalid.
        liarena_free(arena);

        return 0;
    }
*/
#include <stdint.h>

#ifndef LIGHT_ARENA_MAX_RESERVED_VIRTUAL_SPACE_GB
#define LIGHT_ARENA_MAX_RESERVED_VIRTUAL_SPACE_GB 16
#endif

#if defined(_WIN32) || defined(_WIN64)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#ifdef _DEBUG
#define ASSERT(X) if(!(X)) __debugbreak()
#else
#define ASSERT(X)
#endif

#elif defined(__linux__)

#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#ifdef _DEBUG
#include <assert.h>
#define ASSERT(X) assert(X)
#else
#define ASSERT(X)
#endif

#endif

typedef struct Light_Arena_t {
    size_t capacity;    /* the current committed memory capacity of the arena */
    size_t reserved;    /* how much virtual address space is reserved to the arena */
    size_t page_size;   /* the page size of the system */
    void*  ptr;         /* pointer to the base memory of the arena */
} Light_Arena;

/* Create an arena with default LIGHT_ARENA_MAX_RESERVED_VIRTUAL_SPACE_GB Gigabytes of max space. */
Light_Arena* liarena_create(void);

/* Create an arena with 'max_size_gb' Gigabytes of max space. */
Light_Arena* liarena_create_custom(size_t max_size_gb);

/* Allocates 'size_bytes' bytes aligned to 8 bytes by default. */
void* liarena_alloc(Light_Arena* arena, size_t size_bytes);

/* Free's the arena completely */
void  liarena_free(Light_Arena* arena);

/* Resets all the allocations in the arena, but doesn't release its memory back to the OS */
void  liarena_clear(Light_Arena* arena);

/* Releases all the extra memory not needed currently in the arena to the OS. */
void  liarena_trim(Light_Arena* arena);

/* Allocates 'size_bytes' bytes unaligned. */
void* liarena_alloc_unaligned(Light_Arena* arena, size_t size_bytes);

/* Allocates 'size_bytes' bytes unaligned to a specified custom alignment. */
void* liarena_alloc_aligned(Light_Arena* arena, size_t size_bytes, size_t alignment);

#if defined(LIGHT_ARENA_IMPLEMENT)

static size_t liarena_align_delta(char* offset, size_t align_to)
{
	return((align_to - ((size_t)offset % align_to)) % align_to);
}

#if defined(_WIN32) || defined(_WIN64)
Light_Arena* liarena_create_custom(size_t max_size_gb)
{
    uint64_t gigabyte = 1024*1024*1024;
    Light_Arena* arena = (Light_Arena*)VirtualAlloc(0, gigabyte * max_size_gb, MEM_RESERVE, PAGE_NOACCESS);
        
    if(arena)
    {
        SYSTEM_INFO info = {0};
        GetSystemInfo(&info);   

        size_t page_size = info.dwPageSize; 

        // Allocate 2 pages as a start
        if(VirtualAlloc(arena, 2 * page_size, MEM_COMMIT, PAGE_READWRITE))
        {
            arena->page_size = page_size;
            ASSERT(arena->page_size > sizeof(Light_Arena) && arena->page_size > 0);

            arena->capacity = 2 * page_size;
            arena->ptr = (char*)arena + arena->page_size;

            arena->reserved = gigabyte * LIGHT_ARENA_MAX_RESERVED_VIRTUAL_SPACE_GB;
        }
        else
        {
            // Could not allocate anything apparently, system is out of resources. Fail completely.
            VirtualFree(arena, arena->reserved, MEM_RELEASE);
            arena = 0;
        }
    }

    return arena;
}

void* liarena_alloc_unaligned(Light_Arena* arena, size_t size_bytes)
{    
    size_t allocated = (char*)arena->ptr - (char*)arena;
    void* result = arena->ptr;

    if(arena->capacity < (allocated + size_bytes))
    {
        size_t new_size = size_bytes + liarena_align_delta((char*)size_bytes, arena->page_size);
        // Commit the new space
        if(VirtualAlloc((char*)arena + arena->capacity, new_size, MEM_COMMIT, PAGE_READWRITE))
            arena->capacity += new_size;
        else
            return 0; // Could not commit pages, out of memory!
    }

    arena->ptr = (char*)result + size_bytes;

    return result;
}

inline void liarena_free(Light_Arena* arena)
{
    VirtualFree(arena, arena->reserved, MEM_RELEASE);
}

inline void liarena_trim(Light_Arena* arena)
{
    VirtualFree((char*)arena + arena->capacity, arena->reserved - arena->capacity, MEM_DECOMMIT);
}

#elif defined(__linux__)
Light_Arena* liarena_create_custom(size_t max_size_gb)
{
    uint64_t gigabyte = 1024*1024*1024;
    int zerofile = open("/dev/zero", 0);

    Light_Arena* arena = 0;

    if(zerofile != 0)
    {
        arena = (Light_Arena*)mmap(0, gigabyte * max_size_gb, PROT_NONE, MAP_PRIVATE, zerofile, 0);
            
        if(arena)
        {
            size_t page_size = getpagesize(); 

            // Allocate 2 pages as a start        
            if(mprotect(arena, 2 * page_size, PROT_READ|PROT_WRITE) == 0)
            {
                arena->page_size = page_size;
                ASSERT(arena->page_size > sizeof(Light_Arena) && arena->page_size > 0);

                arena->capacity = 2 * page_size;
                arena->ptr = (char*)arena + arena->page_size;

                arena->reserved = gigabyte * LIGHT_ARENA_MAX_RESERVED_VIRTUAL_SPACE_GB;
            }
            else
            {
                // Could not allocate anything apparently, system is out of resources. Fail completely.
                munmap(arena, arena->reserved);
                arena = 0;
            }
        }
    }

    return arena;
}

void* liarena_alloc_unaligned(Light_Arena* arena, size_t size_bytes)
{    
    size_t allocated = (char*)arena->ptr - (char*)arena;
    void* result = arena->ptr;

    if(arena->capacity < (allocated + size_bytes))
    {
        size_t new_size = size_bytes + liarena_align_delta((char*)size_bytes, arena->page_size);
        // Commit the new space
        if(mprotect((char*)arena + arena->capacity, new_size, PROT_READ|PROT_WRITE) == 0)
            arena->capacity += new_size;
        else
            return 0; // Could not commit pages, out of memory!
    }

    arena->ptr = (char*)result + size_bytes;

    return result;
}

inline void liarena_free(Light_Arena* arena)
{
    munmap(arena, arena->reserved);
}

inline void liarena_trim(Light_Arena* arena)
{
    mmap((char*)arena + arena->capacity, arena->reserved - arena->capacity, PROT_NONE, MAP_FIXED|MAP_PRIVATE|MAP_ANON, -1, 0);
    msync((char*)arena + arena->capacity, arena->reserved - arena->capacity, MS_SYNC|MS_INVALIDATE);
}
#endif

Light_Arena* liarena_create(void)
{
    return liarena_create_custom(LIGHT_ARENA_MAX_RESERVED_VIRTUAL_SPACE_GB);
}

inline void* liarena_alloc_aligned(Light_Arena* arena, size_t size_bytes, size_t alignment)
{    
    size_t extra_size = liarena_align_delta(arena->ptr, alignment);
    return liarena_alloc_unaligned(arena, size_bytes + extra_size);
}

inline void* liarena_alloc(Light_Arena* arena, size_t size_bytes)
{
    // Align to 8 bytes
    size_t extra_size = ((8 - ((size_t)(arena->ptr) & 0x7)) & 0x7);
    return liarena_alloc_unaligned(arena, size_bytes + extra_size);
}

inline void liarena_clear(Light_Arena* arena)
{
    arena->ptr = (char*)arena + arena->page_size;
}

#endif
#endif /* H_LIGHT_ARENA */