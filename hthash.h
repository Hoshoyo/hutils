#ifndef HTHASH_
#define HTHASH_

#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <wmmintrin.h>
#include <immintrin.h>

/*  Define HT_IMPLEMENTATION in one of your compilation units
	Define HT_LINKED_LIST_GROW for a linked list version

	Example usage:

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

	// --------------------------------------------------

	Example using c string keys:

	#define HT_IMPLEMENTATION
	#include "hthash.h"

	int main()
	{
		// Create table
		HtTable table = { 0 };
		ht_new(&table, 0, sizeof(int));

		// Add values
		int values[] = { 0,1,2,3,4,5 };
		ht_add_c(&table, "Value0", &values[0]);
		ht_add_c(&table, "Value1", &values[1]);
		ht_add_c(&table, "Value2", &values[2]);
		ht_add_c(&table, "Value3", &values[3]);
		ht_add_c(&table, "Value4", &values[4]);
		ht_add_c(&table, "Value5", &values[5]);

		// Delete a value
		ht_delete_c(&table, "Value4");

		// Find a value
		int* value = ht_get_c(&table, "Value2");
		printf("value is %d\n", *value);

		// Iterate through all values
		for (HtIterator it = { 0 }; value = ht_next(&table, &it);)
		{
			printf("%lld: %d\n", it.i, *value);
		}

		return 0;
	}
*/
#if defined(__cplusplus)
extern "C" {
#endif

#if defined (_WIN32) || defined(_WIN64)
#ifndef PF_AVX_INSTRUCTIONS_AVAILABLE
#define PF_AVX_INSTRUCTIONS_AVAILABLE 39
#endif
#if !defined(_WINDOWS_)
__declspec(dllimport) int __stdcall IsProcessorFeaturePresent(uint32_t feature);
#endif
#endif

#define HT_DEFAULT_INITIAL_SIZE 64
#define HT_DEFAULT_OCCUPANCY 0.7f		/* 70% */
#define HT_DEFAULT_GROWTH_FACTOR 1.0f	/* 100% */

typedef struct {
	size_t capacity;    /* the current committed memory capacity of the arena */
	void*  base;        /* pointer to the base memory of the arena */
	void*  at;
} HtArena;

typedef struct {
	uint64_t i;
	uint64_t at;
	void*    key;
	uint32_t keysize_bytes;
} HtIterator;

typedef struct {
	uint64_t hash;
	void* key;
	uint32_t keysize_bytes;
	uint32_t flags;
#ifdef HT_LINKED_LIST_GROW
	int32_t  next_index;
#endif
	char     data[0];
} HtEntry;

typedef struct {
	HtEntry* entries;
	uint64_t entry_count;
	uint64_t table_size;
	uint32_t entry_size_bytes;
	uint32_t flags;
	float    occupancy;
	float    growth_factor;
#ifdef HT_LINKED_LIST_GROW
	HtEntry* spill_entries_start;
	uint64_t spill_entries_size;
	uint64_t spill_entry_count;
	uint32_t spill_next_free_index;
#endif
	int(*keyequal)(const char*, const char*, uint32_t);
	uint64_t(*hashfunc)(void*, uint32_t);
	void* (*growfunc)(uint64_t);

	HtArena key_arena;

#ifdef HT_STATISTICS
	uint64_t grow_count;
	uint64_t add_collision_count;
	uint64_t lookup_collision_count;
#endif
} HtTable;

/* Does not allow the table to grow, instead if it were to grow, just return zero from ht_add */
#define HTABLE_DISABLE_GROW (1 << 0)
#define HTABLE_DONT_COPY_KEYS (1 << 1)

/* Creates a new hash table where the element size is 'entry_size' and the initial size is HT_DEFAULT_INITIAL_SIZE */
void  ht_new(HtTable* table, uint32_t flags, uint32_t entry_size);

/* Creates a new hash table with a specified initial size */
void  ht_new_sized(HtTable* table, uint32_t flags, uint32_t entry_size, uint64_t initial_count);

/* Creates a hash table where:
- 'occupancy' is the percentage of entries that can occupy the table before it grows automatically.
- 'growth_factor' is the percentage added to the new table (i.e. 1.0f is 100%).
- 'hashfunc' is the hash function used in the key. If 0, uses the default one.
- 'keyequal' is the function used to compare keys. If 0, compares all bytes in the keys for equality.
- 'storage' is the memory used by the table to store the entries.
- 'storage_size' is the size of the storage passed. It is used to calculate how many entries the table will have.
- 'growfunc' the function used to allocate a new table in case it needs to grow. The memory given by this function must be zero initialized.
*/
void ht_new_ex(HtTable* table, uint32_t flags, uint32_t entry_size, float occupancy, float growth_factor,
	uint64_t(*hashfunc)(void*, uint32_t),
	int(*keyequal)(const char*, const char*, uint32_t),
	void* storage, uint32_t storage_size, void* (*growfunc)(uint64_t));

/* Allocates an entry in the hash table without copying the value, returns the pointer to the memory allocated in the table */
void* ht_alloc(HtTable* table, const char* key, int keysize_bytes);

/* Adds an entry to the table and copies the value in place */
void* ht_add(HtTable* table, const char* key, int keysize_bytes, void* value);

/* Finds the entry in the table and returns its value. If the value does not exist, returns 0. */
void* ht_get(HtTable* table, const char* key, int keysize_bytes);

/* Deletes an entry from the table. This does not free up space, just leaves a tombstone in place of the deleted value.
   This function can therefore make the table filled with unusable entries until it grows again. 
   Returns the object that was deleted, 0 if the object did not exist. */
void* ht_delete(HtTable* table, const char* key, int keysize_bytes);

/* Frees up the memory for the table, this does not need to be called in case the memory and grow function were passed directly.
   Use your own memory management in that case */
void  ht_free(HtTable* table);

/* Given an iterator, returns the next entry in the table */
void* ht_next(HtTable* table, HtIterator* it);

/* Same as ht_add, but assumes the key is a c string */
void* ht_add_c(HtTable* table, const char* key, void* value);

/* Same as ht_get, but assumes the key is a c string */
void* ht_get_c(HtTable* table, const char* key);

/* Same as ht_delete, but assumes the key is a c string */
void  ht_delete_c(HtTable* table, const char* key);

#ifdef HT_IMPLEMENTATION

#define HTABLE_ENTRY_FLAG_OCCUPIED (1 << 0)
#define HTABLE_ENTRY_FLAG_TOMBSTONE (1 << 1)

static uint64_t
ht_internal_hash_fnv1(void* key, uint32_t length)
{
	const char* s = (const char*)key;
	uint64_t hash = 14695981039346656037ULL;
	uint64_t fnv_prime = 1099511628211ULL;
	for (uint64_t i = 0; i < (uint64_t)length; ++i) {
		hash = hash * fnv_prime;
		hash = hash ^ s[i];
	}
	return hash;
}

inline static uint32_t
swap_uint32(uint32_t val)
{
	val = ((val << 8) & 0xFF00FF00) | ((val >> 8) & 0xFF00FF);
	return (val << 16) | (val >> 16);
}

inline static uint64_t
ht_internal_hash(void* key, uint32_t keysize_bytes)
{
	__m128i hash = _mm_set_epi32(0x12345678, 0x12ABCDEF, 0x12345678, 0x12345678);
	for (int i = 0; i < keysize_bytes / sizeof(__m128i); ++i)
	{
		__m128i keypart = _mm_loadu_epi64(key);
		hash = _mm_aesdec_si128(keypart, hash);
		hash = _mm_aesdec_si128(hash, hash);
		key = (char*)key + keysize_bytes;
	}

	uint32_t mask[4] = { 0 };
	uint32_t remainder = keysize_bytes % sizeof(__m128i);

	if (remainder)
	{
		for (int i = 0; i < remainder; ++i)
			((char*)mask)[i] = 0xff;
		mask[0] = swap_uint32(mask[0]);
		mask[1] = swap_uint32(mask[1]);
		mask[2] = swap_uint32(mask[2]);
		mask[3] = swap_uint32(mask[3]);

		__m128i loadmask = _mm_loadu_epi64(mask);
		__m128 keypart = _mm_maskload_ps((const float*)key, loadmask);
		hash = _mm_aesdec_si128(*(__m128i*) & keypart, hash);
		hash = _mm_aesdec_si128(hash, hash);
	}

	return hash.m128i_u64[0];
}

static int
ht_key_equal(const char* k1, const char* k2, uint32_t key_size_bytes)
{
	for (uint32_t i = 0; i < key_size_bytes; ++i)
		if (k1[i] != k2[i]) return 0;
	return 1;
}

static void*
ht_alloc_memory(uint64_t size_bytes)
{
	return calloc(1, size_bytes);
}

void
ht_new_ex(HtTable* table, uint32_t flags, uint32_t entry_size, float occupancy, float growth_factor,
	uint64_t(*hashfunc)(void*, uint32_t),
	int(*keyequal)(const char*, const char*, uint32_t),
	void* storage, uint32_t storage_size, void* (*growfunc)(uint64_t))
{
	table->table_size = storage_size / (entry_size + sizeof(HtEntry));
	table->entry_count = 0;
	table->entries = (HtEntry*)storage;
	table->entry_size_bytes = entry_size;
	table->flags = flags;
	table->occupancy = occupancy;
	table->growth_factor = growth_factor;
	table->growfunc = (growfunc != 0) ? growfunc : ht_alloc_memory;
	table->keyequal = (keyequal != 0) ? keyequal : ht_key_equal;
	
	if (!(flags & HTABLE_DONT_COPY_KEYS))
	{
		table->key_arena.capacity = table->table_size * 32;
		table->key_arena.base = calloc(1, table->key_arena.capacity); /* assume 32 bytes as an average key size */
		table->key_arena.at = table->key_arena.base;
	}
	else
	{
		table->key_arena.capacity = 0;
		table->key_arena.base = 0;
		table->key_arena.at = 0;
	}

#ifdef HT_LINKED_LIST_GROW
	uint64_t total_table_size = table->table_size;
	table->table_size *= 0.8f; /* reserve 20 % of the space for collision spill memory */
	table->spill_entries_size = total_table_size - table->table_size;
	table->spill_entries_start = (HtEntry*)((char*)table->entries + (table->table_size * (entry_size + sizeof(HtEntry))));
	table->spill_next_free_index = 1; /* 0 is reserved to indicate not used */
#endif

#ifdef _WIN32
	/* TODO(psv) : Check for AES features */
	if (IsProcessorFeaturePresent((uint32_t)PF_AVX_INSTRUCTIONS_AVAILABLE))
		table->hashfunc = (hashfunc != 0) ? hashfunc : ht_internal_hash;
	else
		table->hashfunc = (hashfunc != 0) ? hashfunc : ht_internal_hash_fnv1;
#else
	table->hashfunc = (hashfunc != 0) ? hashfunc : ht_internal_hash_fnv1;
#endif

#ifdef HT_STATISTICS
	table->grow_count = 0;;
	table->add_collision_count = 0;
	table->lookup_collision_count = 0;
#endif
}

void
ht_new(HtTable* table, uint32_t flags, uint32_t entry_size)
{
	uint64_t storage_size = HT_DEFAULT_INITIAL_SIZE * (entry_size + sizeof(HtEntry));
	void* initial_storage = calloc(1, storage_size);
	ht_new_ex(table, flags, entry_size, HT_DEFAULT_OCCUPANCY, HT_DEFAULT_GROWTH_FACTOR, 0, 0, initial_storage, storage_size, 0);
}

void
ht_new_sized(HtTable* table, uint32_t flags, uint32_t entry_size, uint64_t initial_count)
{
	uint64_t storage_size = initial_count * (entry_size + sizeof(HtEntry));
	void* initial_storage = calloc(1, storage_size);
	ht_new_ex(table, flags, entry_size, HT_DEFAULT_OCCUPANCY, HT_DEFAULT_GROWTH_FACTOR, 0, 0, initial_storage, storage_size, 0);
}

static void
ht_grow(HtTable* table, float factor)
{
	uint64_t final_capacity = (uint64_t)(table->table_size * (1 + factor));
	uint32_t entry_size = ((sizeof(HtEntry) + table->entry_size_bytes));

	uint64_t new_storage_size = final_capacity * entry_size;
	void* new_storage = table->growfunc(new_storage_size);

	HtTable new_table = { 0 };
	ht_new_ex(&new_table, table->flags, table->entry_size_bytes, table->occupancy,
		table->growth_factor, table->hashfunc, table->keyequal, new_storage, new_storage_size, table->growfunc);

	/* copy all the values */
	void* value = 0;
	for (HtIterator it = { 0 }; value = ht_next(table, &it);)
	{
		HtEntry* entry = (HtEntry*)((char*)value - offsetof(HtEntry, data));
		if (entry->keysize_bytes <= sizeof(void*))
		{
			void* key = entry->key;
			ht_add(&new_table, (const char*)&key, entry->keysize_bytes, value);
		}
		else
		{
			ht_add(&new_table, (const char*)entry->key, entry->keysize_bytes, value);
		}
	}
#ifdef HT_STATISTICS	
	new_table.grow_count = table->grow_count + 1;
#endif

	ht_free(table);
	*table = new_table;
}

static uint32_t
ht_probe_start(HtTable* table, uint64_t hash)
{
	return 1 + hash % (table->table_size - 1);
}

static void*
ht_arena_copy(HtTable* table, void* key, int keysize_bytes)
{	
	uint64_t current_arena_size = (char*)table->key_arena.at - (char*)table->key_arena.base;
	if (current_arena_size + keysize_bytes > table->key_arena.capacity)
	{
		/* Allocate more space in the arena */
		table->key_arena.capacity *= 2;
		table->key_arena.base = realloc(table->key_arena.base, table->key_arena.capacity);
		table->key_arena.at = (char*)table->key_arena.base + current_arena_size;
	}

	void* result = table->key_arena.at;
	memcpy(result, key, keysize_bytes);
	table->key_arena.at = (char*)table->key_arena.at + keysize_bytes;

	return result;
}

#ifdef HT_LINKED_LIST_GROW

// TODO(psv): Make this faster
static HtEntry*
find_next_free_entry(HtTable* table, int* find_index)
{
	int32_t index = table->spill_next_free_index;
	uint32_t entry_size = ((sizeof(HtEntry) + table->entry_size_bytes));
	HtEntry* entry = (HtEntry*)((char*)table->spill_entries_start + (index * entry_size));

	if (!entry->flags)
	{
		*find_index = index;
		return entry;
	}
	else
	{
		for (int32_t i = 1; i < table->spill_entries_size; ++i)
		{
			HtEntry* entry = (HtEntry*)((char*)table->spill_entries_start + (i * entry_size));
			if (!entry->flags)
			{
				*find_index = i;
				return entry;
			}
		}
	}
	assert(0); /* Should be unreachable */
	return 0;
}

void*
ht_alloc(HtTable* table, const char* key, int keysize_bytes)
{
	uint64_t hash = table->hashfunc((void*)key, keysize_bytes);
	if ((table->entry_count + 1) > (uint64_t)(table->table_size * table->occupancy) || (table->spill_entry_count + 1) > table->spill_entries_size)
	{
		/* should grow */
		if (table->flags & HTABLE_DISABLE_GROW)
			return 0;
		ht_grow(table, table->growth_factor);
	}

	uint64_t index = hash % table->table_size;

	uint32_t entry_size = ((sizeof(HtEntry) + table->entry_size_bytes));
	HtEntry* entry = (HtEntry*)((char*)table->entries + index * entry_size);
	if (entry->flags & HTABLE_ENTRY_FLAG_OCCUPIED)
	{
		do {
			if (entry->hash == hash && keysize_bytes == entry->keysize_bytes)
			{
				/* Check if the entry is the same and overwrite it */
				switch (keysize_bytes)
				{
					case 8: if ((uint64_t)entry->key == *(uint64_t*)key) return entry->data; break;
					case 4: if ((uint32_t)entry->key == *(uint32_t*)key) return entry->data; break;
					case 2: if ((uint16_t)entry->key == *(uint16_t*)key) return entry->data; break;
					case 1: if ((uint8_t)entry->key == *(uint8_t*)key) return entry->data; break;
					default: {
						if (table->keyequal(key, (const char*)entry->key, keysize_bytes))
							return entry->data;
					} break;
				}
			}

#ifdef HT_STATISTICS
			table->add_collision_count++;
#endif		
			if (entry->next_index <= 0)
			{
				/* Could not find the entry, so we need to allocate it */
				int32_t free_index = 0;
				HtEntry* new_entry = find_next_free_entry(table, &free_index);
				table->spill_entry_count++;
				entry->next_index = free_index;
				entry = new_entry;
				break;
			}

			entry = (HtEntry*)((char*)table->spill_entries_start + (entry->next_index * entry_size));
		} while (entry->flags & HTABLE_ENTRY_FLAG_OCCUPIED);

		/* this entry can now be used */
	}

	switch (keysize_bytes)
	{
		case 8: entry->key = (void*)*(uint64_t*)key; break;
		case 4: entry->key = (void*)*(uint32_t*)key; break;
		case 2: entry->key = (void*)*(uint16_t*)key; break;
		case 1: entry->key = (void*)*(uint8_t*)key; break;
		default: {
			if (table->flags & HTABLE_DONT_COPY_KEYS)
				entry->key = (void*)key;
			else
				entry->key = ht_arena_copy(table, (void*)key, keysize_bytes);
		} break;
	}
	entry->keysize_bytes = keysize_bytes;
	entry->flags = HTABLE_ENTRY_FLAG_OCCUPIED;
	entry->hash = hash;
	entry->next_index = 0;

	table->entry_count++;

	return entry->data;
}
#else
void*
ht_alloc(HtTable* table, const char* key, int keysize_bytes)
{
	uint64_t hash = table->hashfunc((void*)key, keysize_bytes);
	if ((table->entry_count + 1) > (uint64_t)(table->table_size * table->occupancy))
	{
		/* should grow */
		if (table->flags & HTABLE_DISABLE_GROW)
			return 0;
		ht_grow(table, table->growth_factor);
	}

	uint64_t index = hash % table->table_size;

	uint32_t entry_size = ((sizeof(HtEntry) + table->entry_size_bytes));
	HtEntry* entry = (HtEntry*)((char*)table->entries + index * entry_size);
	if (entry->flags & (HTABLE_ENTRY_FLAG_OCCUPIED|HTABLE_ENTRY_FLAG_TOMBSTONE))
	{
		if (entry->flags & HTABLE_ENTRY_FLAG_TOMBSTONE)
		{
			/* Search forward in case it is already in the table */
			void* e = ht_get(table, key, keysize_bytes);
			if (e)
				return e;
		}

		/* probe forward for an empty slot */
		uint32_t probe = ht_probe_start(table, hash);
		do {
			if (entry->hash == hash && keysize_bytes == entry->keysize_bytes)
			{
				/* Check if the entry is the same and overwrite it */
				switch (keysize_bytes)
				{
					case 8: if ((uint64_t)entry->key == *(uint64_t*)key) return entry->data; break;
					case 4: if ((uint32_t)entry->key == *(uint32_t*)key) return entry->data; break;
					case 2: if ((uint16_t)entry->key == *(uint16_t*)key) return entry->data; break;
					case 1: if ((uint8_t)entry->key == *(uint8_t*)key) return entry->data; break;
					default: {
						if (table->keyequal(key, (const char*)entry->key, keysize_bytes))
							return entry->data;
					} break;
				}
			}

#ifdef HT_STATISTICS
			table->add_collision_count++;
#endif
			index = (index + probe) % table->table_size;
			probe++;
			entry = (HtEntry*)((char*)table->entries + index * entry_size);
		} while (entry->flags & HTABLE_ENTRY_FLAG_OCCUPIED);
	}

	switch (keysize_bytes)
	{
		case 8: entry->key = (void*)*(uint64_t*)key; break;
		case 4: entry->key = (void*)*(uint32_t*)key; break;
		case 2: entry->key = (void*)*(uint16_t*)key; break;
		case 1: entry->key = (void*)*(uint8_t*)key; break;
		default: {
			if (table->flags & HTABLE_DONT_COPY_KEYS)
				entry->key = (void*)key;
			else
				entry->key = ht_arena_copy(table, (void*)key, keysize_bytes);
		}break;
	}
	entry->keysize_bytes = keysize_bytes;
	entry->flags = HTABLE_ENTRY_FLAG_OCCUPIED;
	entry->hash = hash;

	table->entry_count++;

	return entry->data;
}
#endif

void*
ht_add(HtTable* table, const char* key, int keysize_bytes, void* value)
{
	void* entry = ht_alloc(table, key, keysize_bytes);
	memcpy(entry, value, table->entry_size_bytes);
	return entry;
}

#ifdef HT_LINKED_LIST_GROW
void*
ht_get(HtTable* table, const char* key, int keysize_bytes)
{
	uint64_t hash = table->hashfunc((void*)key, keysize_bytes);
	uint64_t index = hash % table->table_size;

	uint32_t entry_size = ((sizeof(HtEntry) + table->entry_size_bytes));
	HtEntry* entry = (HtEntry*)((char*)table->entries + index * entry_size);

	while (entry->flags & (HTABLE_ENTRY_FLAG_OCCUPIED|HTABLE_ENTRY_FLAG_TOMBSTONE))
	{
		if (entry->hash == hash && keysize_bytes == entry->keysize_bytes)
		{
			switch (keysize_bytes)
			{
				case 8: if ((uint64_t)entry->key == *(uint64_t*)key) return entry->data; break;
				case 4: if ((uint32_t)entry->key == *(uint32_t*)key) return entry->data; break;
				case 2: if ((uint16_t)entry->key == *(uint16_t*)key) return entry->data; break;
				case 1: if ((uint8_t)entry->key == *(uint8_t*)key) return entry->data; break;
				default: {
					if (table->keyequal(key, (const char*)entry->key, keysize_bytes))
						return entry->data;
				} break;
			}
		}
#ifdef HT_STATISTICS
		table->lookup_collision_count++;
#endif
		if (entry->next_index == 0)
			return 0;
		entry = (HtEntry*)((char*)table->spill_entries_start + (entry->next_index * entry_size));
	}

	return 0;
}
#else
void*
ht_get(HtTable* table, const char* key, int keysize_bytes)
{
	uint64_t hash = table->hashfunc((void*)key, keysize_bytes);
	uint64_t index = hash % table->table_size;

	uint32_t entry_size = ((sizeof(HtEntry) + table->entry_size_bytes));
	HtEntry* entry = (HtEntry*)((char*)table->entries + index * entry_size);

	uint32_t probe = ht_probe_start(table, hash);
	while (entry->flags & (HTABLE_ENTRY_FLAG_OCCUPIED|HTABLE_ENTRY_FLAG_TOMBSTONE))
	{
		if (entry->hash == hash && keysize_bytes == entry->keysize_bytes)
		{
			switch (keysize_bytes)
			{
				case 8: if ((uint64_t)entry->key == *(uint64_t*)key) return entry->data; break;
				case 4: if ((uint32_t)entry->key == *(uint32_t*)key) return entry->data; break;
				case 2: if ((uint16_t)entry->key == *(uint16_t*)key) return entry->data; break;
				case 1: if ((uint8_t)entry->key == *(uint8_t*)key) return entry->data; break;
				default: {
					if (table->keyequal(key, (const char*)entry->key, keysize_bytes))
						return entry->data;
				} break;
			}
		}

#ifdef HT_STATISTICS
		table->lookup_collision_count++;
#endif
		index = (index + probe) % table->table_size;
		probe++;
		entry = (HtEntry*)((char*)table->entries + index * entry_size);
	}

	return 0;
}
#endif

#ifdef HT_LINKED_LIST_GROW
void*
ht_delete(HtTable* table, const char* key, int keysize_bytes)
{
	void* value = ht_get(table, key, keysize_bytes);
	if (value)
	{
		HtEntry* entry = (HtEntry*)((char*)value - offsetof(HtEntry, data));
		uint32_t entry_size = ((sizeof(HtEntry) + table->entry_size_bytes));
		
		if (value >= table->spill_entries_start)
		{
			uint64_t spill_index = ((char*)entry - (char*)table->spill_entries_start) / entry_size;
			HtEntry* next_entry = (HtEntry*)((char*)table->spill_entries_start + (entry->next_index * entry_size));

			/* deleting from spill */
			if ((entry->next_index != 0) && next_entry->flags)
			{
				*entry = *next_entry;
				memcpy(entry->data, next_entry->data, table->entry_size_bytes);
			}
			else
			{							
				entry->flags = 0;
				entry->next_index = 0;
				table->spill_next_free_index = spill_index;
			}
		}
		else
		{
			int32_t spill_index = entry->next_index;
			/* deleting from base table */
			if (spill_index != 0)
			{
				/* put the spill into this slot */
				HtEntry* spill_entry = (HtEntry*)((char*)table->spill_entries_start + (spill_index * entry_size));
				if (spill_entry->flags)
				{
					*entry = *spill_entry;
					memcpy(entry->data, spill_entry->data, table->entry_size_bytes);

					/* update the next free */
					spill_entry->flags = 0;
					table->spill_next_free_index = spill_index;
				}
				else
				{
					entry->flags = 0;
					entry->next_index = 0;
				}
			}
			else
			{
				entry->flags = 0;
				entry->next_index = 0;
			}
		}
		table->entry_count--;	
		return value;	
	}
	return 0;
}
#else
void*
ht_delete(HtTable* table, const char* key, int keysize_bytes)
{
	void* value = ht_get(table, key, keysize_bytes);
	if (value)
	{
		HtEntry* entry = (HtEntry*)((char*)value - offsetof(HtEntry, data));
		entry->flags = HTABLE_ENTRY_FLAG_TOMBSTONE;
		entry->keysize_bytes = 0;
		table->entry_count--;
		return value;
	}
	return 0;
}
#endif

void
ht_free(HtTable* table)
{
	free(table->entries);
	if (table->key_arena.base)
	{
		free(table->key_arena.base);
		table->key_arena.base = 0;
		table->key_arena.at = 0;
		table->key_arena.capacity = 0;
	}
	table->table_size = 0;
	table->entries = 0;
}

void*
ht_add_c(HtTable* table, const char* key, void* value)
{
	return ht_add(table, key, strlen(key), value);
}

void*
ht_get_c(HtTable* table, const char* key)
{
	return ht_get(table, key, strlen(key));
}

void
ht_delete_c(HtTable* table, const char* key)
{
	ht_delete(table, key, strlen(key));
}

#ifdef HT_LINKED_LIST_GROW
void*
ht_next(HtTable* table, HtIterator* it)
{
	uint32_t entry_size = ((sizeof(HtEntry) + table->entry_size_bytes));
	HtEntry* entry = 0;

	if (it->at < table->table_size)
	{
		do {
			entry = (HtEntry*)((char*)table->entries + it->at * entry_size);
			it->at = (it->at + 1);
			if (it->at >= table->table_size)
				break;
		} while (!(entry->flags & HTABLE_ENTRY_FLAG_OCCUPIED) || (!entry->flags));
		if (entry->flags & HTABLE_ENTRY_FLAG_OCCUPIED && entry->flags)
		{
			it->i++;
			it->key = entry->key;
			it->keysize_bytes = entry->keysize_bytes;
			return entry->data;
		}
	}

	if (table->spill_entry_count == 0)
		return 0;

	if (it->at >= table->table_size)
	{
		uint64_t spill_index = it->at - table->table_size + 1;
		do {
			entry = (HtEntry*)((char*)table->spill_entries_start + spill_index * entry_size);
			it->at = (it->at + 1);
			spill_index++;
			if (spill_index >= table->spill_entries_size)
				return 0;
		} while (!(entry->flags & HTABLE_ENTRY_FLAG_OCCUPIED) || (!entry->flags));
	}

	it->i++;
	it->key = entry->key;
	it->keysize_bytes = entry->keysize_bytes;
	return entry->data;
}
#else
void*
ht_next(HtTable* table, HtIterator* it)
{
	uint32_t entry_size = ((sizeof(HtEntry) + table->entry_size_bytes));
	HtEntry* entry = 0;
	do {
		entry = (HtEntry*)((char*)table->entries + it->at * entry_size);
		it->at = (it->at + 1);
		it->i++;
		if (it->at > table->table_size)
			return 0;
	} while (!(entry->flags & HTABLE_ENTRY_FLAG_OCCUPIED) || (entry->flags & HTABLE_ENTRY_FLAG_TOMBSTONE));

	return entry->data;
}
#endif
#endif /* HT_IMPLEMENTATION */

#if defined(__cplusplus)
}
#endif /* extern "C" */

#endif /* HTHASH_ */
