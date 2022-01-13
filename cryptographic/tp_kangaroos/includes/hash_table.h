
#ifndef __HASH_TABLE___
#define __HASH_TABLE___

#include "mul11585.h"
#include "uthash.h" // https://troydhanson.github.io/uthash/

typedef struct trap {
	uint128_t x; // hash table key
						 // x = g^exponent for the tame kangaroo, h *g^exponent
						 // for the wild kangaroo
	uint64_t exponent;
	UT_hash_handle hh; /* makes this structure hashable */
} trap;

trap *new_trap(num128 x, uint64_t exponent);
void delete_all(trap *traps);
void print_trap(trap *trap_entry);

#endif
