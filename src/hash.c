//
// This code was not originally developed by me but by github user
// @tonious
// The repo was cloned and will be modified for use of my project for COSC 455
// Link to hash.c repo:
// https://gist.github.com/tonious/1377667
// I added in a generic implentation for that data that is held in each entry_s element and a function ptr to free that 
// data, functions for properly deallocating the structs given a HT. I've also fixed bugs in the ht_hash function
// ( the hashval was not initalized and caused a lot of issues ).

#define _XOPEN_SOURCE 500 /* Enable certain library functions (strdup) on linux.  See feature_test_macros(7) */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

#include "hash.h"

typedef struct entry_s 
{
	char *key;
	void *value;
	struct entry_s *next;
} entry_t;

/* Hash a string for a particular hash table. */
static int ht_hash( hashtable_t *hashtable, char *key ) {

	unsigned long int hashval = 0;
	int i = 0;

	/* Convert our string to an integer */
	while( hashval < ULONG_MAX && i < strlen( key ) ) {
		hashval = hashval << 8;
		hashval += key[ i ];
		i++;
	}

	return hashval % hashtable->size;
}

/* Create a key-value pair. */
static entry_t *ht_newpair( char *key, void *value, int value_size) {
	entry_t *newpair;

	if( ( newpair = malloc( sizeof( entry_t ) ) ) == NULL ) 
    {
		return NULL;
	}

	if( ( newpair->key = strdup( key ) ) == NULL ) 
    {
		return NULL;
	}

	if( ( newpair->value = malloc(value_size) ) == NULL ) 
    {
		return NULL;
	}

    newpair->value = memcpy(newpair->value, value, value_size);
	newpair->next = NULL;

	return newpair;
}

/* Insert a key-value pair into a hash table. */
void ht_insert( hashtable_t *hashtable, char *key, void *value ) {
	int bin = 0;
	entry_t *newpair = NULL;
	entry_t *next = NULL;
	entry_t *last = NULL;

	bin = ht_hash( hashtable, key );

	next = hashtable->table[ bin ];

	while( next != NULL && next->key != NULL && strcmp( key, next->key ) > 0 ) {
		last = next;
		next = next->next;
	}

	/* There's already a pair.  Let's replace that string. */
	if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) 
    {
        if (hashtable->free_table_entry_data_cb)
        {
            hashtable->free_table_entry_data_cb(next->value);
        }

		next->value = memcpy(next->value, value, hashtable->element_size);
	} 
    else 
    {
	    /* Nope, could't find it.  Time to grow a pair. */

		newpair = ht_newpair( key, value, hashtable->element_size);
        // Add in new key to hashtable list of keys

		/* We're at the start of the linked list in this bin. */
		if( next == hashtable->table[ bin ] ) {
			newpair->next = next;
			hashtable->table[ bin ] = newpair;
	
		/* We're at the end of the linked list in this bin. */
		} else if ( next == NULL ) {
			last->next = newpair;
	
		/* We're in the middle of the list. */
		} else  {
			newpair->next = next;
			last->next = newpair;
		}
	}
}

/* Retrieve a key-value pair from a hash table. */
void *ht_find( hashtable_t *hashtable, char *key ) 
{
	int bin = 0;
	entry_t *pair;

	bin = ht_hash( hashtable, key );

	/* Step through the bin, looking for our value. */
	pair = hashtable->table[ bin ];
	while( pair != NULL && pair->key != NULL && strcmp( key, pair->key ) > 0 ) {
		pair = pair->next;
	}

	/* Did we actually find anything? */
	if( pair == NULL || pair->key == NULL || strcmp( key, pair->key ) != 0 ) {
		return NULL;

	} else {
		return pair->value;
	}
	
}

/* Create a new hashtable. */
hashtable_t *ht_create_new(int size, int element_size, freeFunction free_table_entry_data_cb) 
{
	hashtable_t *hashtable = NULL;
	int i;

	if( size < 1 || element_size < 1) 
    {
        return NULL;
    }

	/* Allocate the table itself. */
	if( ( hashtable = malloc( sizeof( hashtable_t ) ) ) == NULL ) 
    {
		return NULL;
	}

	/* Allocate pointers to the head nodes. */
	if( ( hashtable->table = malloc( sizeof( entry_t * ) * size ) ) == NULL ) 
    {
		return NULL;
	}

	for( i = 0; i < size; i++ ) 
    {
		hashtable->table[i] = NULL;
	}
    
    hashtable->element_size = element_size;
    hashtable->free_table_entry_data_cb = free_table_entry_data_cb;
	hashtable->size = size;
	
    return hashtable;	
}

static void ht_entry_free(entry_t *bucket, freeFunction free_table_entry_data_cb)
{
    entry_t *current = NULL;

    if (bucket)
    {
        current = bucket;
        while (current != NULL)
        {
            if (current->key)
            {
                free(current->key);
            }

            if (current->value)
            {
                if (free_table_entry_data_cb)
                {
                    free_table_entry_data_cb(current->value);
                }

                free(current->value);
            }
            
            current = current->next;
        }
    }
}

void ht_free(hashtable_t *tb)
{
    int i;

    if (tb)
    {
        // Free our table
        for( i = 0; i < tb->size; i++ ) 
        {
            if (tb->table[i])
            {
                // Free the entry stored at the table
                // then free the pointer in the table
                ht_entry_free(tb->table[i], tb->free_table_entry_data_cb);
                free(tb->table[i]);
            }
        }
        
        free(tb);
    }
}
