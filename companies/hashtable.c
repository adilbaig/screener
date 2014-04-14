/**
 * Copied from https://gist.github.com/tonious/1377667
 */

#define _XOPEN_SOURCE 500 /* Enable certain library functions (strdup) on linux.  See feature_test_macros(7) */

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>

struct hashtable_entry {
	char *key;
	char *value;
	struct hashtable_entry *next;
};

struct hashtable {
	size_t size;
	struct entry_s **table;
};

/* Create a new hashtable. */
struct hashtable* ht_create( size_t size ) {

	struct hashtable *t = malloc(sizeof(struct hashtable));
	if(!t) {
		perror("ht_create: malloc failed");
		exit(1);
	}

	*t = (struct hashtable){};
	t->table = malloc(sizeof(struct hashtable_entry)*size);

	/* Allocate pointers to the head nodes. */
	if( t->table == NULL ) {
		perror("ht_create: hashtable->table malloc failed");
		exit(1);
	}

	for(int i = 0; i < size; i++ ) {
		t->table[i] = NULL;
	}

	t->size = size;
	return t;
}

/* Hash a string for a particular hash table. */
size_t ht_hash( struct hashtable *hashtable, char *key ) {

	size_t hashval;
	int i = 0;

	/* Convert our string to an integer */
	while( hashval < ULONG_MAX && i < strlen( key ) ) {
		hashval = hashval << 8;
		hashval += key[i];
		i++;
	}

	return hashval % hashtable->size;
}

/* Create a key-value pair. */
struct hashtable_entry* ht_new(char *key, char *value) {
	struct hashtable_entry* newpair = malloc(sizeof(struct hashtable_entry));

	if(!newpair) {
		return NULL;
	}

	if( ( newpair->key = strdup( key ) ) == NULL ) {
		return NULL;
	}

	if( ( newpair->value = strdup( value ) ) == NULL ) {
		return NULL;
	}

	newpair->next = NULL;

	return newpair;
}

/* Insert a key-value pair into a hash table. */
void ht_set( struct hashtable *hashtable, char *key, char *value ) {
	int bin = 0;
	struct hashtable_entry* newpair = NULL;
	struct hashtable_entry* next = NULL;
	struct hashtable_entry* last = NULL;

	bin = ht_hash( hashtable, key );

	next = hashtable->table[ bin ];

	while( next != NULL && next->key != NULL && strcmp( key, next->key ) > 0 ) {
		last = next;
		next = next->next;
	}

	/* There's already a pair.  Let's replace that string. */
	if( next != NULL && next->key != NULL && strcmp( key, next->key ) == 0 ) {

		free( next->value );
		next->value = strdup( value );

	/* Nope, could't find it.  Time to grow a pair. */
	} else {
		newpair = ht_new( key, value );

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
char *ht_get( struct hashtable *hashtable, char *key ) {
	int bin = 0;
	struct hashtable_entry* pair;

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


int main( int argc, char **argv ) {

	struct hashtable* hashtable = ht_create( 65536 );

	ht_set( hashtable, "key1", "inky" );
	ht_set( hashtable, "key2", "pinky" );
	ht_set( hashtable, "key3", "blinky" );
	ht_set( hashtable, "key4", "floyd" );
	ht_set( hashtable, "key1", "REPLACED inky" );

	printf( "%s\n", ht_get( hashtable, "key1" ) );
	printf( "%s\n", ht_get( hashtable, "key2" ) );
	printf( "%s\n", ht_get( hashtable, "key3" ) );
	printf( "%s\n", ht_get( hashtable, "key4" ) );

	return 0;
}
