#pragma once

/*

	#define DYNAMIC_ARRAY_IMPLEMENT for implementations
	
	#define DYNAMIC_ARRAY_NO_CRT to not include C runtime libraries and headers, but the
	user needs to define the following functions as for the CRT specification:
		- malloc	(size_t size_bytes)
		- realloc	(void* ptr, size_t size_bytes)
		- free		(void* ptr)
		- memcpy	(void* dest, void* src, size_t size_bytes)
	all of them with the same name.
	
	Linkage is extern by default,
	#define DYNAMIC_ARRAY_STATIC for static implementation, meaning one for each translation unit
	
*/

#ifndef DYNAMIC_ARRAY_NO_CRT
	#include <stdlib.h>
	#include <string.h>
#endif

#if defined(_WIN64) || defined(_WIN32)
	#define HO_API __stdcall
#else
	#define HO_API
#endif

#ifdef DYNAMIC_ARRAY_STATIC
	#define HO_EXPORT static
#else
	#ifdef __cplusplus
	#define HO_EXPORT extern "C"
	#else
	#define HO_EXPORT extern
	#endif
#endif

// array_create		: creates a dynamic array of the type specified and reserves (see array_reserve)
// the number of elements specified. Do not use the memory reserved without calling array_push, array_emplace
// or array_allocate.
#define array_create(type, num) (type*)_array_create(num, sizeof(type))

// array_push		: allocates and copies 1 element at the end of the array
#define array_push(arr, data) _array_push((void**)&arr, data)

// array_emplace	: allocates memory for 1 element at the end of the array but does not copy data into it.
// Also returns the index from the start of the array of the element allocated.
#define array_emplace(arr) _array_emplace((void**)&arr)

// array_allocate	: allocates the memory for the amount of elements passed, doesn't fill the memory.
#define array_allocate(arr, num) _array_allocate((void**)&arr, num)

// array_reserve	: preallocates memory for the number of elements passed, do not use the memory,
// as it is only reserved and not commited. Use array_allocate to reserve and commit. This function
// is purely optimization of allocation for permormance.
#define array_reserve(arr, num) _array_reserve((void**)&arr, num)

HO_EXPORT size_t  HO_API array_get_capacity(void* array_);
HO_EXPORT size_t  HO_API array_get_length(void* array_);
HO_EXPORT size_t  HO_API array_get_element_size(void* array_);

// array_pop		: removes the last element of the array if it exists and returns it.
HO_EXPORT   void* HO_API array_pop(void* array);

// array_release	: frees all memory of the array
HO_EXPORT   void  HO_API array_release(void* array);

// array_remove		: removes the element and replaces it with the last element in the array
// if that exists. Unordered remove!
HO_EXPORT   void  HO_API array_remove(void* array_, size_t index);

// array_clear		: clears all the elements in the array but keeps the current capacity allocated.
HO_EXPORT   void  HO_API array_clear(void* array_);

// _array_create
// _array_push
// _array_emplace
// use the array_create macro instead
HO_EXPORT   void  HO_API _array_allocate(void** array_, size_t num_elements);
HO_EXPORT   void* HO_API _array_create(int num_elements, size_t size);
HO_EXPORT   void  HO_API _array_push(void** array_, void* data);
HO_EXPORT size_t  HO_API _array_emplace(void** array_);

#if defined(DYNAMIC_ARRAY_IMPLEMENT)

typedef struct dynamic_array_t {
	size_t capacity;
	size_t length;
	size_t size_element;
} dynamic_array;

HO_EXPORT void* HO_API array_pop(void* array_)
{
	dynamic_array* base = (dynamic_array*)((char*)array_ - sizeof(dynamic_array));
	size_t size_element = base->size_element;
	if (base->length > 0)
		base->length--;
	return (char*)array_ + size_element * base->length;
}

HO_EXPORT void HO_API array_release(void* array_)
{
	dynamic_array* base = (dynamic_array*)((char*)array_ - sizeof(dynamic_array));
	free(base);
}

HO_EXPORT void HO_API array_remove(void* array_, size_t index)
{
	dynamic_array* base = (dynamic_array*)((char*)array_ - sizeof(dynamic_array));
	size_t capacity = base->capacity;
	size_t length = base->length;
	size_t size_element = base->size_element;

	void* dst = (char*)array_ + index * size_element;
	void* from = (char*)array_ + ((length - 1) * size_element);
	memcpy(dst, from, size_element);
	base->length--;
}

HO_EXPORT void* HO_API _array_create(int num_elements, size_t size)
{
	void* arr = malloc(sizeof(dynamic_array) + num_elements * size);
	dynamic_array* aux = (dynamic_array*)arr;
	aux->capacity = num_elements;
	aux->length = 0;
	aux->size_element = size;
	return (char*)arr + sizeof(dynamic_array);
}

HO_EXPORT void HO_API _array_push(void** array_, void* data)
{
	dynamic_array* base = (dynamic_array*)((char*)*array_ - sizeof(dynamic_array));
	size_t capacity = base->capacity;
	size_t length = base->length;
	size_t size_element = base->size_element;

	if (length == capacity)
	{
		capacity *= 2;
		base->capacity = capacity;

		void* new_mem = realloc(base, capacity * size_element + sizeof(dynamic_array));
		base = (dynamic_array*)new_mem;
		*array_ = (char*)new_mem + sizeof(dynamic_array);
	}

	void* dst = (char*)*array_ + length * size_element;
	base->length++;
	memcpy(dst, data, size_element);
}

HO_EXPORT size_t HO_API _array_emplace(void** array_)
{
	dynamic_array* base = (dynamic_array*)((char*)*array_ - sizeof(dynamic_array));
	size_t capacity = base->capacity;
	size_t length = base->length;
	size_t size_element = base->size_element;

	if (length == capacity)
	{
		capacity *= 2;
		base->capacity = capacity;
		void* new_mem = realloc(base, capacity * size_element + sizeof(dynamic_array));
		base = (dynamic_array*)new_mem;
		*array_ = (char*)new_mem + sizeof(dynamic_array);
	}

	void* dst = (char*)*array_ + length * size_element;
	base->length++;

	return length;
}

HO_EXPORT void HO_API _array_allocate(void** array_, size_t num_elements) 
{
	dynamic_array* base = (dynamic_array*)((char*)*array_ - sizeof(dynamic_array));
	size_t capacity = base->capacity;
	size_t length = base->length;
	size_t size_element = base->size_element;

	if (length == capacity || length + num_elements > capacity)
	{
		capacity += num_elements;
		base->capacity = capacity;

		void* new_mem = realloc(base, capacity * size_element + sizeof(dynamic_array));
		base = (dynamic_array*)new_mem;
		*array_ = (char*)new_mem + sizeof(dynamic_array);
	}
	base->length += num_elements;
}

HO_EXPORT void HO_API _array_reserve(void** array_, size_t num_elements)
{
	dynamic_array* base = (dynamic_array*)((char*)*array_ - sizeof(dynamic_array));
	size_t capacity = base->capacity;
	size_t length = base->length;
	size_t size_element = base->size_element;

	if (length == capacity || length + num_elements > capacity)
	{
		capacity += num_elements;
		base->capacity = capacity;

		void* new_mem = realloc(base, capacity * size_element + sizeof(dynamic_array));
		base = (dynamic_array*)new_mem;
		*array_ = (char*)new_mem + sizeof(dynamic_array);
	}
}

HO_EXPORT size_t HO_API array_get_capacity(void* array_)
{
	return ((dynamic_array*)((char*)array_ - sizeof(dynamic_array)))->capacity;
}

HO_EXPORT size_t HO_API array_get_length(void* array_)
{
	return ((dynamic_array*)((char*)array_ - sizeof(dynamic_array)))->length;
}

HO_EXPORT size_t HO_API array_get_element_size(void* array_)
{
	return ((dynamic_array*)((char*)array_ - sizeof(dynamic_array)))->size_element;
}

HO_EXPORT void HO_API array_clear(void* array_)
{
	((dynamic_array*)((char*)array_ - sizeof(dynamic_array)))->length = 0;
}
#endif	// DYNAMIC_ARRAY_IMPLEMENT

#undef HO_API
