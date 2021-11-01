#include <stdio.h>

#include "hash_table.h"

/*
 * new_hash_entry returns a new hash_msg with key @h and message @m.
 */
hash_msg *new_hash_entry(uint64_t h, uint32_t m[4]) {
	hash_msg *ht = malloc(sizeof(*ht));
	if (ht == NULL) {
		fprintf(stderr, "couldn't allocate the hash table\n");

		return NULL;
	}

	ht->h = h;

	for (uint8_t i = 0; i < 4; ++i) {
		ht->m[i] = m[i];
	}

	return ht;
}

/*
 * delete_all deletes and frees all the entries of @hash_table.
 */
void delete_all(hash_msg *hash_table) {
	hash_msg *current_hash, *tmp;

	HASH_ITER(hh, hash_table, current_hash, tmp) {
		HASH_DEL(hash_table,
				 current_hash); /* delete it (hash_table advances to next) */
		free(current_hash);		/* free it */
		current_hash = NULL;
	}
}

/*
 * print_hash_msg prints @hash_table_entry on the terminal.
 */
void print_hash_msg(hash_msg *hash_table_entry) {
	printf("\nkey: %lu\n", hash_table_entry->h);
	for (uint8_t i = 0; i < 4; ++i) {
		printf("%d", hash_table_entry->m[i]);
	}
}