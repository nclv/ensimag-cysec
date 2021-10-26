#include "hash_table.h"

hash_msg *new_hash_entry(uint64_t h, uint32_t m[4]) {
	hash_msg *ht = malloc(sizeof(*ht));

	ht->h = h;

	for (uint8_t i = 0; i < 4; ++i) {
		ht->m[i] = m[i];
	}

	return ht;
}

void delete_all(hash_msg *hash_table) {
	hash_msg *current_hash, *tmp;

	HASH_ITER(hh, hash_table, current_hash, tmp) {
		HASH_DEL(hash_table,
				 current_hash); /* delete it (hash_table advances to next) */
		free(current_hash);		/* free it */
	}
}