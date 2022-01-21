#include <stdio.h>

#include "hash_table.h"
#include "mul11585.h"

/*
 * new_hash_entry returns a new trap with key @h and message @m.
 */
trap *new_trap(num128 x, uint64_t exponent) {
	trap *t = malloc(sizeof(*t));
	if (t == NULL) {
		fprintf(stderr, "couldn't allocate the hash table\n");
		return NULL;
	}

	(t->x).t[0] = x.t[0];
	(t->x).t[1] = x.t[1];
	t->exponent = exponent;

	return t;
}

/*
 * delete_all deletes and frees all the entries of @hash_table.
 */
void delete_all(trap *hash_table) {
	trap *current_hash, *tmp;

	HASH_ITER(hh, hash_table, current_hash, tmp) {
		HASH_DEL(hash_table,
				 current_hash); /* delete it (hash_table advances to next) */
		free(current_hash);		/* free it */
		current_hash = NULL;
	}
}

/*
 * print_trap prints @hash_table_entry on the terminal.
 */
void print_trap(trap *trap_entry) {
	//num128 n = {.s = trap_entry->x};

	printf("(");
	//print_num128(n);
	print_num128(trap_entry->x);
	printf(" , %lx)", trap_entry->exponent);
}

void print_traps(trap *traps) {
	trap *current_trap, *tmp;

	HASH_ITER(hh, traps, current_trap, tmp) {
		print_trap(current_trap);
		printf("-");
	}
	printf("\n");
}