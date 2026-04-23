#ifndef HT_H
#define HT_H

/**
 * Hash table implementation in C, created using Ben Hoyt's
 * great article found here: https://benhoyt.com/writings/hash-table-in-c/
 */

typedef struct ht ht;

/**
 * Void param, in C means explicitly no parameters
 */
ht *ht_create(void);

void ht_destroy(ht *table);

void *ht_get(ht *table, const char *key);

const char *ht_set(ht *table, const char *key, void *value);

#endif