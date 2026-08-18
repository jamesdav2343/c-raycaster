#ifndef HT_H
#define HT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#ifndef u_int64_t
typedef uint64_t u_int64_t;
#endif

/**
 * Hash table implementation in C, created using Ben Hoyt's
 * great article found here: https://benhoyt.com/writings/hash-table-in-c/
 */

/**
 * Hash table struct. Created with ht_create, destroyed with ht_destroy.
 */
typedef struct ht ht;

/**
 * Creates the hash table.
 *
 * Note: void param, in C means explicitly no parameters.
 */
ht* ht_create(void);

/**
 * Frees the hash table.
 */
void ht_destroy(ht* table);

/**
 * Get a value by the given key.
 */
void* ht_get(ht* table, const char* key);

/**
 * Set the given key and value.
 */
const char* ht_set(ht* table, const char* key, void* value);

/**
 * Returns the number of items in the has table.
 */
size_t ht_length(ht* table);

/**
 * Hash table iterator.
 */
typedef struct {
    const char* key; // current key
    void* value; // current value

    // Don't use these fields directly.
    ht* _table; // reference to hash table being iterated
    size_t _index; // current index into ht._entries
} hti;

/**
 * Return new hash table iterator (for use with ht_next).
 *
 */
hti ht_iterator(ht* table);

/**
 * Move iterator to next item in hash table, update iterator's key
 * and value to current item, and return true. If there are no more
 * items, return false. Don't call ht_set during iteration.
 */
bool ht_next(hti* it);

/**
 * Prints the hash table keys and addresses of values.
 */

void ht_print_debug(ht* table);

#endif