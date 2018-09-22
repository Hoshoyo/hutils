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

## [Light Arena](https://github.com/Hoshoyo/hutils/blob/master/light_arena.h)

A fast memory arena allocator library for `C/C++`.

```c
#include "light_arena.h"

int main(int argc, char** argv)
{
    Light_Arena* arena = arena_create(512);

    void* m1 = arena_alloc(arena, 64);
    void* m2 = arena_alloc(arena, 84);

    arena_free(arena);

    return 0;
}
```

## [HoGL](https://github.com/Hoshoyo/hutils/blob/master/ho_gl.h)

A minimal OpenGL extensions library for `C++`.

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

## Ho Math (Deprecated)

A math library for `C++`.

## Dynamic Array (Deprecated)

A fast dynamic array library for `C/C++`.
