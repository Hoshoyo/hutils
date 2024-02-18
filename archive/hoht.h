#pragma once
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Define HOHT_IMPLEMENTATION in one of your compilation units

    Example usage:

    #define HOHT_IMPLEMENTATION
    #include "hoht.h"

    int main() {
        Hoht_Table table = {0};
        hoht_new(&table, 10, sizeof(int), 0.5f, malloc, free);

        int a = 1;
        int b = 2;
        int c = 3;

        hoht_push(&table, "Hello", &a);
        hoht_push(&table, "Hello", &b);
        hoht_push(&table, "Hello", &c);

        int a_res;
        hoht_get(&table, "Hello", &a_res);

        // will print 1
        printf("a is: %d\n", a_res);
    }
*/

/* Define HOHT_PRINTING_IMPLEMENTATION for printing functions */
#if defined(HOHT_PRINTING_IMPLEMENTATION)
#include <stdio.h>
#endif

/* Define HOHT_SERIALIZE_IMPLEMENTATION for serialization functions */
#if defined(HOHT_SERIALIZE_IMPLEMENTATION)
#include <stdio.h>
#endif

typedef struct {
    int   entry_count;
    int   capacity;
    int   entry_size_bytes;
    float occupancy_ratio;
    float occupancy_max;

    int   (*print_entry)(void*);
    void* (*allocator)(size_t size);
    void  (*freer)(void*);

    void* entries;
} Hoht_Table;

/*  macro that returns hash entry value dereferenced from table(T), type(K) and index(I) */
#define HOHT_GET_ENTRY_FROM_INDEX(T, K, I) (*(K*)((Hoht_Table_Entry*)((char*)(T)->entries + (sizeof(Hoht_Table_Entry) + (T)->entry_size_bytes) * (I)))->entry)

/*  macro that returns hash entry value pointer as void* from table(T), type(TYPE) and key(KEY) */
#define HOHT_GET_ENTRY(T, TYPE, KEY) (*(TYPE*)hoht_get_value(T, KEY))

/*  hoht_new
    creates a new hash table with capacity number of max entries.
    'entry_size_bytes' is the size of the type stored inside the table
    and defines the size of the table element, this size must be constant
    throughout the lifetime of the table.
    'occupancy' is the ratio at which the table will grow automatically
    when reached, i.e. occupancy 0.5 will trigger a grow call when 
    the table reaches 50% of its capacity. allocator and freer must
    be provided to use internally for memory allocation, malloc and free
    can be use directly. */
int hoht_new(Hoht_Table* table, int capacity, int entry_size_bytes, float occupancy, void*(*allocator)(size_t size), void(*freer)(void*));

/*  hoht_free
    frees resources from a hash table created with hoht_new */
int hoht_free(Hoht_Table* table);

/*  hoht_grow
    is used internally to grow the hash table by a factor given. It can
    be used by the user to trigger an early grow but it is not recommended.
    This is an expensive operation O(N). */
int hoht_grow(Hoht_Table* table, int factor);

/*  hoht_push 
    inserts a value to the hash table, this can trigger a table grow call
    if the occupancy max is reached, otherwise is an O(1) operation. 
    returns index in the table of the inserted value.
    The key must be zero terminated, if it is not, use hoht_push_length or
    hoht_push_hashed instead */
int hoht_push(Hoht_Table* table, const char* key, void* value);
int hoht_push_length(Hoht_Table* table, const char* key, int key_length, void* value);
int hoht_push_hashed(Hoht_Table* table, uint64_t hash, void* value);

/*  hoht_get
    retreives from the hash table the index and value in out (must be allocated by caller), 
    returns -1 if the key does not exist in the hash table.
    given a key, this operation is always O(1).
    The key must be zero terminated, if it is not, use hoht_get_length or
    hoht_get_hashed instead */
int hoht_get(Hoht_Table* table, const char* key, void* out);
int hoht_get_length(Hoht_Table* table, const char* key, int key_length, void* out);
int hoht_get_hashed(Hoht_Table* table, uint64_t hash, void* out);

/*  hoht_get_value
    retreives the entry pointer as a void* given a key if the entry exists in the hash, 
    0 (NULL) otherwise. 
    The key must be zero terminated, if it is not, use hoht_get_value_length or
    hoht_get_value_hashed instead */
void* hoht_get_value(Hoht_Table* table, const char* key);
void* hoht_get_value_length(Hoht_Table* table, const char* key, int key_length);
void* hoht_get_value_hashed(Hoht_Table* table, uint64_t hash);

/*  hoht_get_value_from_index
    retreives the entry pointer as a void* given the hash index if the entry exists in the hash, 
    0 (NULL) otherwise. */
void* hoht_get_value_from_index(Hoht_Table* table, int index);

/*  hoht_delete
    deletes from the hash table the value correspondent to the key, returns -1 if the
    value doesn't exist, 0 otherwise. This operation is always O(1). 
    The key must be zero terminated, if it is not, use hoht_delete_length or
    hoht_delete_hashed instead */
int hoht_delete(Hoht_Table* table, const char* key);
int hoht_delete_length(Hoht_Table* table, const char* key, int key_length);
int hoht_delete_hashed(Hoht_Table* table, uint64_t hash);

/*  hoht_serialize
    serializes the hash table in a file with given filename */
int hoht_serialize(const char* filename, Hoht_Table* table);

/*  hoht_load_from_file
    loads into out_table a table serialized using hoht_serialize, this function
    allocates memory for entries using the allocator passed. */
int hoht_load_from_file(const char* filename, Hoht_Table* out_table, void*(*allocator)(size_t), void(*freer)(void*));

#if defined(HOHT_IMPLEMENTATION)

static unsigned int HASH_TABLE_OCCUPIED = (1 << 0);

typedef struct Hoht_Table_Entry {
    uint32_t flags;
    uint64_t hash;
    struct Hoht_Table_Entry* next; /* colliding entries in a linked list */
    int entry[0];
} Hoht_Table_Entry;

static uint64_t
hoht_fnv_1_hash(const char* s, uint64_t length) {
    uint64_t hash = 14695981039346656037ULL;
    uint64_t fnv_prime = 1099511628211ULL;
    for (uint64_t i = 0; i < length; ++i) {
        hash = hash * fnv_prime;
        hash = hash ^ s[i];
    }
    return hash;
}

int 
hoht_new(Hoht_Table* table, int capacity, int entry_size_bytes, float occupancy, void*(*allocator)(size_t size), void(*freer)(void*)) {
    table->entry_size_bytes = entry_size_bytes;
    table->capacity = capacity;
    table->entry_count = 0;
    table->entries = allocator((sizeof(Hoht_Table_Entry) + entry_size_bytes) * capacity);
    table->occupancy_max = occupancy;
    table->allocator = allocator;
    table->freer = freer;
    memset(table->entries, 0, (sizeof(Hoht_Table_Entry) + entry_size_bytes) * capacity);
    return 0;
}

int 
hoht_grow(Hoht_Table* table, int factor) {
    if(factor <= 0) return -1;
    int final_capacity = table->capacity * factor;

    Hoht_Table new_table = {0};
    hoht_new(&new_table, final_capacity, table->entry_size_bytes, table->occupancy_max, table->allocator, table->freer);
    new_table.print_entry = table->print_entry;

    for(int i = 0; i < table->capacity; ++i) {
        Hoht_Table_Entry* entry_ptr = (Hoht_Table_Entry*)((char*)table->entries + (sizeof(Hoht_Table_Entry) + table->entry_size_bytes) * i);

        if(entry_ptr->flags & HASH_TABLE_OCCUPIED) {
            hoht_push_hashed(&new_table, entry_ptr->hash, (void*)entry_ptr->entry);
            Hoht_Table_Entry* entry = entry_ptr->next;
            while(entry) {
                hoht_push_hashed(&new_table, entry->hash, entry->entry);
                entry = entry->next;
            }
        }
    }

    hoht_free(table);
    *table = new_table;

    return 0;
}

int 
hoht_free(Hoht_Table* table) {
    for(int i = 0; i < table->capacity; ++i) {
        Hoht_Table_Entry* entry_ptr = (Hoht_Table_Entry*)((char*)table->entries + (sizeof(Hoht_Table_Entry) + table->entry_size_bytes) * i);
        if(entry_ptr->flags & HASH_TABLE_OCCUPIED) {
            Hoht_Table_Entry* entry = entry_ptr->next;
            while(entry) {
                Hoht_Table_Entry* prev = entry;
                entry = entry->next;
                table->freer(prev);
            }
        }
    }
    table->freer(table->entries);
    return 0;
}

int 
hoht_push_hashed(Hoht_Table* table, uint64_t hash, void* value) {
    if(table->occupancy_ratio >= table->occupancy_max) hoht_grow(table, 2);

    int index = (int)(hash % (uint64_t)table->capacity);

    Hoht_Table_Entry* entry_ptr = (Hoht_Table_Entry*)((char*)table->entries + (sizeof(Hoht_Table_Entry) + table->entry_size_bytes) * index);
    Hoht_Table_Entry* entry = entry_ptr;
    if(entry_ptr->flags & HASH_TABLE_OCCUPIED) {
        if (entry_ptr->hash == hash && memcmp(entry->entry, value, table->entry_size_bytes) == 0) {
            /* same entry */
            return index;
        } else {
            /* new entry */
            Hoht_Table_Entry* prev = entry_ptr;
            while(prev->next != 0) {
                prev = prev->next;
            }
            size_t alloc_size = sizeof(Hoht_Table_Entry) + table->entry_size_bytes;
            prev->next = (Hoht_Table_Entry*)table->allocator(alloc_size);
            memset(prev->next, 0, alloc_size);
            entry = prev->next;
        }
    }
    entry->flags = HASH_TABLE_OCCUPIED;
    entry->hash = hash;
    memcpy(entry->entry, value, table->entry_size_bytes);

    table->entry_count++;
    table->occupancy_ratio = (float)(table->entry_count) / (float)table->capacity;
    
    return index;
}

int 
hoht_push(Hoht_Table* table, const char* key, void* value) {
    uint64_t hash = hoht_fnv_1_hash(key, strlen(key));
    return hoht_push_hashed(table, hash, value);
}

int
hoht_push_length(Hoht_Table* table, const char* key, int key_length, void* value) {
    uint64_t hash = hoht_fnv_1_hash(key, key_length);
    return hoht_push_hashed(table, hash, value);
}

int
hoht_get_hashed(Hoht_Table* table, uint64_t hash, void* out) {
    int index = (int)(hash % (uint64_t)table->capacity);
    
    Hoht_Table_Entry* entry_ptr = (Hoht_Table_Entry*)((char*)table->entries + (sizeof(Hoht_Table_Entry) + table->entry_size_bytes) * index);
    if(entry_ptr->flags & HASH_TABLE_OCCUPIED) {
        if(entry_ptr->hash == hash) {
            if(out) {
                memcpy(out, entry_ptr->entry, table->entry_size_bytes);
            }
            return index;
        } else {
            Hoht_Table_Entry* entry = entry_ptr->next;
            while(entry) {
                if(entry->hash == hash) {
                    if(out) {
                        memcpy(out, entry->entry, table->entry_size_bytes);
                    }
                    return index;
                }
                entry = entry->next;
            }
            return -1;
        }
    } else {
        return -1;
    }
}

int
hoht_get_length(Hoht_Table* table, const char* key, int key_length, void* out) {
    uint64_t hash = hoht_fnv_1_hash(key, strlen(key));
    return hoht_get_hashed(table, hash, out);
}

int
hoht_get(Hoht_Table* table, const char* key, void* out) {
    return hoht_get_length(table, key, strlen(key), out);
}

void*
hoht_get_value_hashed(Hoht_Table* table, uint64_t hash) {
    int index = (int)(hash % (uint64_t)table->capacity);
    Hoht_Table_Entry* entry_ptr = (Hoht_Table_Entry*)((char*)table->entries + (sizeof(Hoht_Table_Entry) + table->entry_size_bytes) * index);
    if(entry_ptr->flags & HASH_TABLE_OCCUPIED) {
        if(entry_ptr->hash == hash) {
            return entry_ptr->entry;
        } else {
            Hoht_Table_Entry* entry = entry_ptr->next;
            while(entry) {
                if(entry->hash == hash) {
                    return entry->entry;
                }
                entry = entry->next;
            }
            return 0;
        }
    } else {
        return 0;
    }
}

void*
hoht_get_value_length(Hoht_Table* table, const char* key, int key_length) {
    uint64_t hash = hoht_fnv_1_hash(key, key_length);
    return hoht_get_value_hashed(table, hash);
}

void*
hoht_get_value(Hoht_Table* table, const char* key) {
    uint64_t hash = hoht_fnv_1_hash(key, strlen(key));
    return hoht_get_value_hashed(table, hash);
}

void*
hoht_get_value_from_index(Hoht_Table* table, int index) {
    Hoht_Table_Entry* entry_ptr = (Hoht_Table_Entry*)((char*)table->entries + (sizeof(Hoht_Table_Entry) + table->entry_size_bytes) * index);
    if(entry_ptr->flags & HASH_TABLE_OCCUPIED) {
        return entry_ptr->entry;
    } else {
        return 0;
    }
}

int hoht_delete_hashed(Hoht_Table* table, uint64_t hash) {
    int index = (int)(hash % (uint64_t)table->capacity);

    Hoht_Table_Entry* entry_ptr = (Hoht_Table_Entry*)((char*)table->entries + (sizeof(Hoht_Table_Entry) + table->entry_size_bytes) * index);

    Hoht_Table_Entry* prev = 0;
    Hoht_Table_Entry* entry = entry_ptr;

    while(entry && entry->hash != hash) {
        prev = entry;
        entry = entry->next;
    }
    if(!entry) {
        /* element does not exist */
        return -1;
    }

    if(prev) {
        prev->next = entry->next;
        table->freer(entry);
    } else {
        if(entry->next) {
            Hoht_Table_Entry* n = entry->next;
            memcpy(entry, entry->next, sizeof(Hoht_Table_Entry) + table->entry_size_bytes);
            entry->next = n->next;
            table->freer(n);
        } else {
            entry->flags = 0;
        }
    }
    return 0;
}

int
hoht_delete_length(Hoht_Table* table, const char* key, int key_length) {
    uint64_t hash = hoht_fnv_1_hash(key, key_length);
    return hoht_delete_hashed(table, hash);
}

int 
hoht_delete(Hoht_Table* table, const char* key) {
    uint64_t hash = hoht_fnv_1_hash(key, strlen(key));
    return hoht_delete_hashed(table, hash);
}


#if defined(HOHT_SERIALIZE_IMPLEMENTATION)
int
hoht_serialize(const char* filename, Hoht_Table* table) {
    FILE* out = fopen(filename, "wb");
    if(out == 0) return -1;

    if(fwrite(table, sizeof(*table), 1, out) != 1) {
        fclose(out);
        return -1;
    }
    size_t bytes_to_write = table->capacity * (table->entry_size_bytes + sizeof(Hoht_Table_Entry));
    if(fwrite(table->entries, bytes_to_write, 1, out) != 1) {
        fclose(out);
        return -1;
    }
    fclose(out);
    return 0;
}

int
hoht_load_from_file(const char* filename, Hoht_Table* out_table, void*(*allocator)(size_t), void(*freer)(void*)) {
    FILE* in = fopen(filename, "rb");
    if(in == 0) return -1;

    fseek(in, 0, SEEK_END);
    long file_size_bytes = ftell(in);
    rewind(in);

    if(fread(out_table, sizeof(*out_table), 1, in) != 1) {
        fclose(in);
        return -1;
    }

    size_t bytes_to_read = out_table->capacity * (out_table->entry_size_bytes + sizeof(Hoht_Table_Entry));
    out_table->entries = allocator(bytes_to_read);
    memset(out_table->entries, 0, bytes_to_read);
    if(fread(out_table->entries, bytes_to_read, 1, in) != 1) {
        fclose(in);
        return -1;
    }
    fclose(in);
    out_table->allocator = allocator;
    out_table->freer = freer;
    return 0;
}
#endif

#if defined(HOHT_PRINTING_IMPLEMENTATION)

static void 
hoht_print_indent_level(int level) {
    for(int i = 0; i < level; ++i)
        printf("\t");
}

static void 
hoht_print_bytes(char* ptr, int length) {
    for(int i = 0; i < length; ++i) {
        printf("%d ", (unsigned char)ptr[i]);
    }
}

int 
hoht_print_entry(Hoht_Table_Entry* entry, int(*print_func)(void*), int entry_size_bytes, int index, int level) {
    hoht_print_indent_level(level);
    printf("Entry[%d]: ", index);
    if(entry->flags & HASH_TABLE_OCCUPIED) {
        printf("<occupied>\n");
        hoht_print_indent_level(level);
        printf("Hash: %lx\n", entry->hash);
        hoht_print_indent_level(level);
        if(print_func) {
            print_func((void*)entry->entry);
        } else {
            printf("[ ");
            hoht_print_bytes((char*)entry->entry, entry_size_bytes);
            printf("]");
        }
        printf("\n");
    } else {
        printf("<free>\n");
    }
    printf("\n");

    if(entry->next) {
        hoht_print_entry(entry->next, print_func, entry_size_bytes, index, level+1);
    }
}

int 
hoht_print(Hoht_Table* table, int indent_level) {
    for(int i = 0; i < table->capacity; ++i) {
        Hoht_Table_Entry* entry_ptr = (Hoht_Table_Entry*)((char*)table->entries + (sizeof(Hoht_Table_Entry) + table->entry_size_bytes) * i);
        hoht_print_entry(entry_ptr, table->print_entry, table->entry_size_bytes, i, indent_level);
    }
}

#endif
#endif