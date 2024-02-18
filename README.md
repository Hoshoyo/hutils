# hutils - Utilities for C/C++ programming

## [Light Array](https://github.com/Hoshoyo/hutils/blob/master/light_array.h)

A fast dynamic array library for `C/C++` similar to C++'s `std::vector`.

```c
#include "light_array.h"

int main(int argc, char** argv)
{
    int* buffer = array_new(int);

    array_push(buffer, 1);  // [1]
    array_push(buffer, 2);  // [1, 2]
    array_push(buffer, 3);  // [1, 2, 3]
    array_push(buffer, 4);  // [1, 2, 3, 4]
    array_push(buffer, 5);  // [1, 2, 3, 4, 5]
    
    int five = array_pop(buffer);       // [1, 2, 3, 4]

    array_remove(buffer, 1);            // [1, 4, 3]
    array_remove_ordered(buffer, 0);    // [4, 3]

    array_free(buffer); 

    return 0;
}
```

---

## [Light Arena](https://github.com/Hoshoyo/hutils/blob/master/liarena.h)

A fast memory arena allocator library for `C/C++`. Reserves a contiguous memory block.

```c
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
```

## [HoGL](https://github.com/Hoshoyo/hutils/blob/master/ho_gl.h)

A minimal OpenGL extensions library for `C/C++`.

```c
#define HOGL_IMPLEMENT // only in one compilation unit
#include "ho_gl.h"

int main(int argc, char** argv) {
    // Initialize OpenGL and get a context
    // hogl_init_opengl is an alternative for windows

    if(hogl_init_gl_extensions() == -1) {
        // Error handling ...
    }

    // Use OpenGL extensions

    return 0;
}
```

## [HtHash](https://github.com/Hoshoyo/hutils/blob/master/hthash.h)

A growing hash table implementation for `C/C++`. 

```c
#include <stdio.h>
#define HT_IMPLEMENTATION
#include "hthash.h"

int main()
{
    // Create table
    HtTable table = { 0 };
    ht_new(&table, 0, sizeof(int));

    // Store integers both as keys and values
    for (int i = 0; i < 10; ++i)
        ht_add(&table, &i, sizeof(int), &i);

    // Delete a value
    int two = 2;
    ht_delete(&table, &two, sizeof(int));

    // Find a value
    int three = 3;
    int* val = ht_get(&table, &three, sizeof(int));
    // Prints 3
    printf("The value is: %d\n", *val);

    // Overwrite a value
    int ten = 10;
    ht_add(&table, &three, sizeof(int), &ten);

    // Iterate through all values
    int* value = 0;
    for (HtIterator it = { 0 }; value = ht_next(&table, &it);)
        printf("%lld: %d\n", it.i, *value);

    return 0;
}
```
