#include "data_structures/ht.h"
#include <assert.h>
#include <stdio.h>
#include <string.h>

#define INITIAL_CAPACITY 16
#define INITIAL_LENGTH 0
#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL

typedef struct {
    const char* key;
    void* value;
} ht_entry;

struct ht {
    ht_entry* entries;
    size_t capacity;
    size_t length;
};

static u_int64_t hash_key(const char* key);
static const char* ht_set_entry(ht_entry* entries, size_t capacity, const char* key, void* value, size_t* table_length);
static bool ht_expand(ht* table);

ht* ht_create(void)
{
    // create the table and return it
    // do some error checking too

    ht* table = (ht*)malloc(sizeof(ht));

    table->capacity = INITIAL_CAPACITY;
    table->length = INITIAL_LENGTH;
    table->entries = (ht_entry*)calloc(table->capacity, sizeof(ht_entry));

    return table;
}

void ht_destroy(ht* table)
{
    for (size_t i = 0; i < table->capacity; i++) {
        free((void*)table->entries[i].key);
    }

    free(table->entries);
    free(table);
}

/**
 * Hash function. Using FNV-1 hash algorithm.
 * https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function#FNV-1a_hash
 */
static u_int64_t hash_key(const char* key)
{
    u_int64_t hash = FNV_OFFSET;

    for (const char* p = key; *p; p++) {
        hash *= FNV_PRIME;
        hash ^= (uint64_t)(unsigned char)*p;
    }

    return hash;
}

void* ht_get(ht* table, const char* key)
{
    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(table->capacity - 1));

    // Loop till we find an empty entry.
    while (table->entries[index].key != NULL) {
        if (strcmp(key, table->entries[index].key) == 0) {
            return table->entries[index].value;
        }

        // Key wasn't in this slot, move to next (linear probing).
        index++;

        // Loop back to start of map
        if (index >= table->capacity) {
            index = 0;
        }
    }

    return NULL;
}

const char* ht_set(ht* table, const char* key, void* value)
{
    assert(value != NULL);
    if (value == NULL) {
        return NULL;
    }

    const size_t HALF_CAPACITY = table->capacity / 2;

    if (table->length >= HALF_CAPACITY) {
        if (!ht_expand(table)) {
            return NULL;
        }
    }

    return ht_set_entry(table->entries, table->capacity, key, value, &table->length);
}

static const char* ht_set_entry(ht_entry* entries, size_t capacity, const char* key, void* value, size_t* table_length)
{
    // AND hash with capacity-1 to ensure it's within entries array.
    uint64_t hash = hash_key(key);
    size_t index = (size_t)(hash & (uint64_t)(capacity - 1));

    // Loop till we find an empty entry.
    while (entries[index].key != NULL) {
        if (strcmp(key, entries[index].key) == 0) {
            entries[index].value = value;
            return entries[index].key;
        }

        // Key wasn't in this slot, move to next (linear probing).
        index++;

        // Loop back to start of map
        if (index >= capacity) {
            index = 0;
        }
    }

    // Didn't find key, allocate+copy if needed, then insert it. Increment item count.
    if (table_length != NULL) {
        key = strdup(key);
        if (key == NULL) {
            return NULL;
        }
        (*table_length)++;
    }

    entries[index].key = (char*)key;
    entries[index].value = value;
    return key;
}

// Expand hash table to twice its current size. Return true on success,
// false if out of memory.
static bool ht_expand(ht* table)
{
    // Allocate new entries array.
    const size_t NEW_CAPACITY = table->capacity * 2;
    if (NEW_CAPACITY < table->capacity) {
        return false; // overflow (capacity would be too big)
    }
    ht_entry* new_entries = calloc(NEW_CAPACITY, sizeof(ht_entry));
    if (new_entries == NULL) {
        return false;
    }

    // Iterate entries, move all non-empty ones to new table's entries.
    for (size_t i = 0; i < table->capacity; i++) {
        ht_entry entry = table->entries[i];
        if (entry.key != NULL) {
            ht_set_entry(new_entries, NEW_CAPACITY, entry.key, entry.value, NULL);
        }
    }

    // Free old entries array and update this table's details.
    free(table->entries);
    table->entries = new_entries;
    table->capacity = NEW_CAPACITY;
    return true;
}

size_t ht_length(ht* table) { return table->length; }

hti ht_iterator(ht* table)
{
    hti it;
    it._table = table;
    it._index = 0;
    return it;
}

bool ht_next(hti* it)
{
    // Loop till we've hit end of entries array.
    ht* table = it->_table;
    while (it->_index < table->capacity) {
        size_t i = it->_index;
        it->_index++;
        if (table->entries[i].key != NULL) {
            // Found next non-empty item, update iterator key and value.
            ht_entry entry = table->entries[i];
            it->key = entry.key;
            it->value = entry.value;
            return true;
        }
    }
    return false;
}

void ht_print_debug(ht* table)
{
    hti it = ht_iterator(table);

    while (ht_next(&it)) {
        printf("%s -> %p\n", it.key, it.value);
    }

    printf("\n");
}