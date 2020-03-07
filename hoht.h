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
    if the occupancy max is reached, otherwise is an O(1) operation. */
int hoht_push(Hoht_Table* table, const char* key, void* value);

/*  hoht_push_hashed
    same as hoht_push but taking the hash from the key directly. */
int hoht_push_hashed(Hoht_Table* table, uint64_t hash, void* value);

/*  hoht_get 
    retreives from the hash table the value, given a key, this operation is always O(1) */
int hoht_get(Hoht_Table* table, const char* key, void* out);

/*  hoht_delete
    deletes from the hash table the value correspondent to the key, returns -1 if the
    value doesn't exist, 0 otherwise. This operation is always O(1). */
int hoht_delete(Hoht_Table* table, const char* key);

#if defined(HOHT_IMPLEMENTATION)

#define HASH_TABLE_OCCUPIED (1 << 0)

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
        if (entry_ptr->hash == hash) {
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
hoht_get(Hoht_Table* table, const char* key, void* out) {
    uint64_t hash = hoht_fnv_1_hash(key, strlen(key));
    int index = (int)(hash % (uint64_t)table->capacity);
    
    Hoht_Table_Entry* entry_ptr = (Hoht_Table_Entry*)((char*)table->entries + (sizeof(Hoht_Table_Entry) + table->entry_size_bytes) * index);
    if(entry_ptr->flags & HASH_TABLE_OCCUPIED) {
        if(entry_ptr->hash == hash) {
            memcpy(out, entry_ptr->entry, table->entry_size_bytes);
            return index;
        } else {
            Hoht_Table_Entry* entry = entry_ptr->next;
            while(entry) {
                if(entry->hash == hash) {
                    memcpy(out, entry->entry, table->entry_size_bytes);
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
hoht_delete(Hoht_Table* table, const char* key) {
    uint64_t hash = hoht_fnv_1_hash(key, strlen(key));
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
}

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

#undef HASH_TABLE_OCCUPIED
#endif