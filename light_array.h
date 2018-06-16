#ifndef H_FAST_DYNAMIC_ARRAY
#define H_FAST_DYNAMIC_ARRAY

/*
    Author: Pedro Sassen Veiga
    This library is C89 compatible
*/

/*
    define LIGHT_ARRAY_NO_CRT if you don't want the c runtime library included
    if that is defined, you must provide implementations for the following functions:
    void* calloc(size_t num, size_t size)
    void* realloc(void* ptr, size_t new_size)
    void free(void* block)
    void* memcpy(void* dest, void* src, size_t count)

    define LIGHT_ARRAY_64BIT for a 64 bit version of the library
*/

#if !defined(LIGHT_ARRAY_NO_CRT)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

/*
    Usage:
    An example usage of creating, pushing, popping, removing and freeing an array:

    #include "light_array.h"

    int main(int argc, char** argv)
    {
        int* buffer = array_new(int);

        array_push(buffer, 1);
        array_push(buffer, 2);
        array_push(buffer, 3);
        array_push(buffer, 4);
        array_push(buffer, 5);
        
        int five = array_pop(buffer);

        array_remove(buffer, 1);
        array_free(buffer);

        return 0;
    }

    array_remove is an unordered remove, if you want an ordered version, which is more expensive
    use array_remove_ordered instead

    To get the length of an array just use array_length, this will work as an lvalue, meaning you
    can do as follows:

    array_length(buffer)++;
    array_length(buffer) = 0;

    And it should work as expected, incrementing the array length artificially or setting it to
    zero, which is also the behaviour of array_clear
*/

typedef struct {
#ifdef LIGHT_ARRAY_64BIT
    unsigned long long capacity;
    unsigned long long length;
#else
    unsigned int capacity;
    unsigned int length;
#endif
} Dynamic_ArrayBase;

#define LIGHT_ARRAY_MIN(x, y) (y ^ ((x ^ y) & -(x < y)))
#define LIGHT_ARRAY_MAX(x, y) (x ^ ((x ^ y) & -(x < y)))

#define array_base(A) ((Dynamic_ArrayBase*)(((char*)(A)) - sizeof(Dynamic_ArrayBase)))
#define array_length(A) array_base(A)->length
#define array_capacity(A) array_base(A)->capacity
#if defined(__cplusplus)
#define array_new(T) (T*)((char*)&(((Dynamic_ArrayBase*)calloc(1, sizeof(Dynamic_ArrayBase) + sizeof(T)))->capacity = 1) + sizeof(Dynamic_ArrayBase))
#else
static void* array_dyn_allocate(size_t size) {
    void* res = calloc(1, size);
    ((Dynamic_ArrayBase*)res)->capacity = 1;
    return (void*)((char*)res + sizeof(Dynamic_ArrayBase));
}
#define array_new(T) array_dyn_allocate(sizeof(T) + sizeof(Dynamic_ArrayBase))
#endif
#define array_push(A, V) ((array_length(A) == array_capacity(A)) ? array_capacity(A) = LIGHT_ARRAY_MAX(2 * array_length(A), 2), \
*((void**)&(A)) = ((char*)realloc(array_base(A), sizeof(Dynamic_ArrayBase) + array_capacity(A) * sizeof(*(A))) + sizeof(Dynamic_ArrayBase)) : 0, \
(A)[array_length(A)++] = (V))
#define array_pop(A) (A)[--array_length(A)]
#define array_free(A) free(array_base(A))
#define array_clear(A) array_length(A) = 0
#define array_remove_ordered(A, Index) (array_length(A)--, \
memcpy(((char*)A) + sizeof(*(A)) * (Index), ((char*)A) + sizeof(*(A)) * ((Index) + 1), sizeof(*buffer) * (array_length(A) - (Index))))
#define array_remove(A, Index) (array_length(A)--, (A)[Index] = (A)[array_length(A)])

#endif /* H_FAST_DYNAMIC_ARRAY */