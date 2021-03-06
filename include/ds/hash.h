#ifndef HASH_TB_H
#define HASH_TB_H

#include "../ds/common.h"
#include "../ds/list.h"

typedef struct hashtable_s 
{
    freeFunction free_table_entry_data_cb;
    struct entry_s **table;
	int size;
    int element_size;
} hashtable_t;


// Inserts new key -> value mapping in the hashtable
// If a mapping already exists value is replaced with the 
// current entry
extern void ht_insert(hashtable_t *hashtable, char *key, void *value);
// Finds and returns the value stored identified by key
extern void *ht_find(hashtable_t *hashtable, char *key);
// Creates a new hashtable object
extern hashtable_t *ht_create_new(int size, int element_size, freeFunction free_table_entry_data_cb);
// Frees the hashtable and all of its resources 
extern void ht_free(hashtable_t *tb);

#endif
