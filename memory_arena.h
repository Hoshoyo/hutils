#pragma once
#include <stdio.h>

#if defined(HO_ARENA_IMPLEMENT)
#if !defined(DYNAMIC_ARRAY_IMPLEMENT)
#define DYNAMIC_ARRAY_IMPLEMENT
#include <dynamic_array.h>
#endif
#else
#include <dynamic_array.h>
#endif

#ifdef __cplusplus
#define HO_ARENA_EXTERN extern "C"
#else
#define HO_ARENA_EXTERN extern
#endif

#ifdef HO_USE_ASSERT
#include <assert.h>
#else
#define assert(X)
#endif

typedef struct {
	size_t size_used;
	void* memory;
	void* current_pointer;
} Arena_Block;

typedef struct {
	size_t total_size;
	size_t block_size;
	int num_allocs;
	int num_blocks;
	int current_block;

	Arena_Block* blocks;
} Memory_Arena;

HO_ARENA_EXTERN void  arena_create(Memory_Arena* arena, size_t size);
HO_ARENA_EXTERN void  arena_release(Memory_Arena* arena);
HO_ARENA_EXTERN void  arena_align_to(Memory_Arena* arena, size_t n);
HO_ARENA_EXTERN void* arena_alloc(Memory_Arena* arena, size_t bytes);

#if defined(HO_ARENA_IMPLEMENT)

void arena_create(Memory_Arena* arena, size_t size) {
	arena->blocks = array_create(Arena_Block, 4);
	size_t index = array_emplace(arena->blocks);
	arena->blocks[index].memory = calloc(1, size);
	arena->blocks[index].current_pointer = arena->blocks[index].memory;
	arena->blocks[index].size_used = 0;

	arena->total_size = 0;
	arena->block_size = size;
	arena->num_allocs = 0;
	arena->num_blocks = 1;
	arena->current_block = index;
}

void arena_release(Memory_Arena* arena) {
	for (int i = 0; i < arena->num_blocks; ++i) {
		free(arena->blocks[i].memory);
	}
	array_release(arena->blocks);
	arena->total_size = 0;
	arena->num_allocs = 0;
	arena->current_block = 0;
}

void arena_align_to(Memory_Arena* arena, size_t n) {
	size_t current_mem = (size_t)arena->blocks[arena->current_block].current_pointer;
	size_t rest = current_mem % n;
	if (arena->blocks[arena->current_block].size_used + rest <= arena->block_size) {
		arena->blocks[arena->current_block].current_pointer = (char*)arena->blocks[arena->current_block].current_pointer + (n - rest);
	}
	else {
		size_t index = array_emplace(arena->blocks);
		arena->current_block += 1;
		arena->num_blocks += 1;
		assert(index == arena->current_block);
		arena->blocks[index].memory = calloc(1, arena->block_size);
		arena->blocks[index].current_pointer = arena->blocks[index].memory;
		arena->blocks[index].size_used = 0;
	}
	size_t aligned = (size_t)arena->blocks[arena->current_block].current_pointer;
	assert(aligned % n == 0);
}

void* arena_alloc(Memory_Arena* arena, size_t bytes) {
	if (bytes > arena->block_size) {
		return 0;
	}
	void* result = 0;
	if (arena->blocks[arena->current_block].size_used + bytes <= arena->block_size) {
		result = arena->blocks[arena->current_block].current_pointer;
		arena->blocks[arena->current_block].size_used += bytes;
		arena->blocks[arena->current_block].current_pointer = ((char*)arena->blocks[arena->current_block].current_pointer) + bytes;
	}
	else {
		size_t index = array_emplace(arena->blocks);
		arena->current_block += 1;
		arena->num_blocks += 1;
		assert(index == arena->current_block);
		arena->blocks[index].memory = calloc(1, arena->block_size);
		arena->blocks[index].current_pointer = ((char*)arena->blocks[index].memory) + bytes;
		arena->blocks[index].size_used = bytes;
		result = arena->blocks[index].memory;
	}

	arena->total_size += bytes;
	arena->num_allocs += 1;

	arena_align_to(arena, sizeof(size_t));
	return result;
}
#undef HO_ARENA_IMPLEMENT
#endif	// HO_ARENA_IMPLEMENT