
// See https://troydhanson.github.io/uthash/userguide.html#hash_functions
//
// https://github.com/PeterScott/murmur3

#include <bits/stdint-uintn.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "kangaroos.h"
#include "mul11585.h"
#include "uthash.h"
#include "xoshiro256starstar.h"

typedef struct position {
	num128 x; // hash table key
	unsigned long long cumulative_exponent;
	UT_hash_handle hh; /* makes this structure hashable */
} position;

position *new_hash_entry(num128 x, unsigned long long cumulative_exponent) {
	position *t = malloc(sizeof(*t));
	if (t == NULL) {
		fprintf(stderr, "couldn't allocate the hash table\n");
		return NULL;
	}

	t->x = x;
	t->cumulative_exponent = cumulative_exponent;

	return t;
}

void delete_all_hash_entries(position *hash_table) {
	position *current_hash, *tmp;

	HASH_ITER(hh, hash_table, current_hash, tmp) {
		HASH_DEL(hash_table,
				 current_hash); /* delete it (hash_table advances to next) */
		free(current_hash);		/* free it */
		current_hash = NULL;
	}
}

void print_hash_entry(position *pos) {
	if (pos == NULL) {
		return;
	}

	// num128 n = {.s = pos->x};
	printf("(");
	print_num128(pos->x);
	printf(" , %lld)\n", pos->cumulative_exponent);
}

/**
 * @brief next_jump_size returns the next jump size of the kangaroo.
 *
 * We use an iterating function determining the next location of the kangaroos:
 * pos_{i+1} = pos_i.g^f(pos_i), where f(pos_i) is a function that outputs the
 * next jump size for the kangaroo and the kangaroo moves forward by this value.
 * We think of the exponents as travel distances, which should be small in
 * comparison with the size of the group (search space).
 *
 * The function f(pos_i) randomly selects values from a set S and only depends
 * on the current location pos_i. In the original paper (Pollard), S is a set
 * containing powers of 2 starting with 2^0 up to a limit with the largest value
 * in the set being such that the mean of the values in the set is a certain
 * (optimized) value µ.
 *
 * Here we choose f(pos_i) = 2^{hash(pos_i) % k + 1} with k the size of the set
 * S.
 *
 * @param x
 * @param k
 * @return uint64_t because 2^(hash % k + 1) maximum value is 2^k, so maximum k
value is 2^63
 */
unsigned long long next_jump_size(num128 *pos, uint8_t k) {
	unsigned hashv;
	HASH_FUNCTION(pos, sizeof(*pos), hashv);
	return (1ULL << ((hashv % k) + 1));
}

/**
 * @brief generate_powers_of_two puts the powers of two from 2^0 to 2^{max - 1}
 * into an array.
 *
 * @param powers_of_two
 * @param max
 */
void generate_powers_of_two(uint64_t *powers_of_two, uint8_t max) {
	for (size_t i = 0; i < max; i++) {
		powers_of_two[i] = (1UL << i);
	}
}

/**
 * @brief A simple way to implement the distinguished point set is to define a
 * point to be distinguished if the n lowest bits in the representation of the
 * point as a binary string are zero. Then the proportion of distinguished
 * points is theta = 1/2^F. In our experiments, we worked with F = 0,...,6,12.
 * Our results fully match the theoretical predictions of how theta effects
 * the running time. In general, F should be chosen small enough such that
 * 1/theta = 2^F is small compared with sqrt(W), but not too small since the
 * algorithm has to store an expected number of 2.theta.sqrt(W) distinguished
 * points.
 *
 * @param position
 * @return true
 * @return false
 */
bool distinguisher(num128 pos) {
	// (log2(2^64) + 1)//2 + 2 = 2^32
	// log2(2^64) / sqrt(2^64) = 1 / 2^26
	// bigger -> less distinguished points
	return pos.s % (1ULL << 26) == 0;
}

void lambda_method(void) {
	uint8_t k = 36;
	uint64_t mu = 1UL << 31;
	// beta = 1.39
	// alpha = sqrt(2^64 * (1 + exp(-beta)) / (2*beta*(2 - exp(-beta))))
	// i.e. approx. 0.51*sqrt(2^64) = 2^31
	// less than 2^28 jumps -> there may be no distinguished elements for the
	// tame
	// if tame makes approximately 0.7.sqrt(2^64) jumps before placing the trap.
	uint64_t number_of_jump_tame =
		(uint64_t)(0.7 * (1UL << 32)); // alpha * beta = mu
	printf("Number of jump tame : %lu\n", number_of_jump_tame);
	// If the expected jump size (exponent values) from a set of jump
	// (exponents) values is µ and the distance that needs to be covered is d
	// then, the number of jumps is given by the following equation:
	// mean jump size x number of jumps = distance covered.
	// Hence, the number of jumps (exponents, i.e. k) needed to cover distance d
	// is d / µ.

	// generate k powers of two
	uint64_t powers_of_two[k];
	generate_powers_of_two(powers_of_two, k);

	uint64_t x = 257; // xoshiro256starstar_random(); // between 0
					  // and 2^63 - 1 ~ 2^64
	num128 h = gexp(x);
	printf("We search x : %lu\ng^x : ", x);
	print_num128(h);
	printf("\n");

	// the tame starts at the middle of the interval i.e. 2^32
	num128 tame = gexp((1UL << 32));
	printf("Tame start : \n");
	print_num128(tame);
	printf("\n");

	// TODO: overflow
	uint64_t cumulative_exponent = 0;

	position *hash_table = NULL; /* important! initialize to NULL */
	position *hi = NULL;

	hi = new_hash_entry(tame, cumulative_exponent);
	print_hash_entry(hi);
	HASH_ADD(hh, hash_table, x, sizeof(num128), hi);
	hi = NULL;

	for (size_t i = 0; i < number_of_jump_tame; i++) {
		uint64_t exponent = next_jump_size(&tame, k);
		// printf("Exponent : %ld\n", exponent);
		tame = mul11585(tame, gexp(exponent));
		// printf("New tame : \n");
		// print_num128(tame);
		// printf("\n");

		cumulative_exponent += exponent;

		if (distinguisher(tame)) {
			printf("Distinguisher tame : \n");
			print_num128(tame);
			printf("\n");

			// Check that this entry was not yet inserted in the hash table
			HASH_FIND(hh, hash_table, &tame, sizeof(num128), hi);
			if (hi == NULL) {
				hi = new_hash_entry(tame, cumulative_exponent);
				HASH_ADD(hh, hash_table, x, sizeof(num128), hi);
			}

			// print_hash_entry(hi);
			hi = NULL;

			if (tame.s == h.s) {
				printf("We reached h at jump n° %ld\n", i);
			}
		}
	}

	uint64_t cumulative_exponent_from_origin =
		(1UL << 32) + cumulative_exponent;
	printf("%ld, %ld\n", cumulative_exponent_from_origin, cumulative_exponent);

	num128 wild = h;
	uint64_t z = 0;
	bool collided = false;

	while (!collided) {
		printf("\nNew wild\n");

		wild = mul11585(wild, gexp(z));
		// printf("New wild : \n");
		// print_num128(wild);
		// printf("\n");

		cumulative_exponent = 0;
		// after about 2.7.sqrt(2^64) jumps of the wild, it either must have
		// fallen into the trap, which happens with probability ≈ 0.75, or it
		// must have safely passed the trap and should be halted.
		uint64_t number_of_jump_wild =
			(uint64_t)(2.7 * (1UL << 32)); // mu + number_of_jump_tame;
		printf("Number of jump wild : %lu\n", number_of_jump_wild);

		for (size_t i = 0; i < number_of_jump_wild; i++) {
			uint64_t exponent = next_jump_size(&wild, k);
			// printf("Exponent : %ld\n", exponent);
			wild = mul11585(wild, gexp(exponent));
			// printf("New wild : \n");
			// print_num128(wild);
			// printf("\n");

			cumulative_exponent += exponent;

			if (cumulative_exponent > cumulative_exponent_from_origin) {
				printf("Wild passed the Tame : %lu, %lu\n", cumulative_exponent,
					   cumulative_exponent_from_origin);
				break;
			}

			if (distinguisher(wild)) {
				printf("Distinguisher wild: \n");
				print_num128(wild);
				printf("\n");

				// Check that this entry was not yet inserted in the hash table
				HASH_FIND(hh, hash_table, &wild, sizeof(num128), hi);
				if (hi != NULL) {
					printf("Wild collided with Tame\n");
					collided = true;
					uint64_t computed_x = (1UL << 32) +
										  hi->cumulative_exponent -
										  cumulative_exponent - z;
					print_hash_entry(hi);
					printf("x : %lu\n", computed_x);
					break;
				}
			}
		}

		z = xoshiro256starstar_random();
	}
}

void lambda_method_simultaneous(void) {
	uint8_t k = 36;
	unsigned long long mu = 1ULL << 31;
	// beta = 1.39
	// alpha = sqrt(2^64 * (1 + exp(-beta)) / (2*beta*(2 - exp(-beta))))
	// i.e. approx. 0.51*sqrt(2^64) = 2^31
	// less than 2^28 jumps -> there may be no distinguished elements for the
	// tame
	// if tame makes approximately 0.7.sqrt(2^64) jumps before placing the trap.
	unsigned long long number_of_jump_tame =
		(unsigned long long)(0.7 * (1ULL << 32)); // alpha * beta = mu
	printf("Number of jump tame : %llu\n", number_of_jump_tame);
	// If the expected jump size (exponent values) from a set of jump
	// (exponents) values is µ and the distance that needs to be covered is d
	// then, the number of jumps is given by the following equation:
	// mean jump size x number of jumps = distance covered.
	// Hence, the number of jumps (exponents, i.e. k) needed to cover distance d
	// is d / µ.

	// after about 2.7.sqrt(2^64) jumps of the wild, it either must have
	// fallen into the trap, which happens with probability ≈ 0.75, or it
	// must have safely passed the trap and should be halted.
	unsigned long long number_of_jump_wild =
		(unsigned long long)(2.7 * (1ULL << 32)); // mu + number_of_jump_tame;
	printf("Number of jump wild : %llu\n", number_of_jump_wild);

	// generate k powers of two
	uint64_t powers_of_two[k];
	generate_powers_of_two(powers_of_two, k);

	uint64_t x = 112123123412345; // xoshiro256starstar_random(); // between 0
								  // and 2^63 - 1 ~ 2^64
	num128 h = gexp(x);
	printf("We search x : %lu\ng^x : ", x);
	print_num128(h);
	printf("\n");

	// the tame starts at the middle of the interval i.e. 2^32
	num128 tame = gexp((1ULL << 32));
	printf("Tame starts at : \n");
	print_num128(tame);
	printf("\n");
	// the wild starts at h
	num128 wild = h;
	printf("Wild starts at : \n");
	print_num128(wild);
	printf("\n");

	unsigned long long exponent = 0;
	unsigned long long tame_cumulative_exponent = 0;
	unsigned long long wild_cumulative_exponent = 0;

	position *hash_table = NULL; /* important! initialize to NULL */
	position *hi = NULL;

	hi = new_hash_entry(tame, tame_cumulative_exponent);
	HASH_ADD(hh, hash_table, x, sizeof(num128), hi);
	hi = NULL;

	bool collided = false;
	size_t jump_count = 0;

	while (!collided) {
		// TAME
		exponent = next_jump_size(&tame, k);
		// printf("Exponent : %ld\n", exponent);
		tame = mul11585(tame, gexp(exponent));
		// printf("New tame : \n");
		// print_num128(tame);
		// printf("\n");

		// Check that this entry was not yet inserted in the hash table
		HASH_FIND(hh, hash_table, &tame, sizeof(num128), hi);
		if (hi != NULL) {
			printf("Tame collided with Wild\n");
			printf("Hash entry : \n");
			print_hash_entry(hi);

			collided = true;
			unsigned long long computed_x = (1ULL << 32) +
											hi->cumulative_exponent -
											tame_cumulative_exponent;

			printf("x : %llu\n", computed_x);
			break;
		}

		if (distinguisher(tame)) {
			printf("Distinguisher tame : \n");
			print_num128(tame);
			printf("\n");
			tame_cumulative_exponent += exponent;

			printf("Tame cumulative exponent : %lld\n",
				   tame_cumulative_exponent);

			// Check that this entry was not yet inserted in the hash table
			HASH_FIND(hh, hash_table, &tame, sizeof(num128), hi);
			if (hi == NULL) {
				hi = new_hash_entry(tame, tame_cumulative_exponent);
				HASH_ADD(hh, hash_table, x, sizeof(num128), hi);
			} else {
				printf("Tame collided with Wild\n");
				printf("Hash entry : \n");
				print_hash_entry(hi);

				collided = true;
				unsigned long long computed_x = (1ULL << 32) +
												hi->cumulative_exponent -
												tame_cumulative_exponent;

				printf("x : %llu\n", computed_x);
				break;
			}

			// print_hash_entry(hi);
			hi = NULL;

			if (tame.s == h.s) {
				printf("We reached h at jump n° %ld\n", jump_count);
			}
		}

		// WILD
		exponent = next_jump_size(&wild, k);
		// printf("Exponent : %ld\n", exponent);
		wild = mul11585(wild, gexp(exponent));
		// printf("New wild : \n");
		// print_num128(wild);
		// printf("\n");

		if (distinguisher(wild)) {
			printf("Distinguisher wild: \n");
			print_num128(wild);
			printf("\n");
			wild_cumulative_exponent += exponent;

			printf("Wild cumulative exponent : %lld\n",
				   wild_cumulative_exponent);

			// Check that this entry was not yet inserted in the hash table
			HASH_FIND(hh, hash_table, &wild, sizeof(num128), hi);
			if (hi == NULL) {
				hi = new_hash_entry(wild, wild_cumulative_exponent);
				HASH_ADD(hh, hash_table, x, sizeof(num128), hi);
			} else {
				printf("Wild collided with Tame\n");
				printf("Hash entry : \n");
				print_hash_entry(hi);

				collided = true;
				unsigned long long computed_x = (1ULL << 32) +
												hi->cumulative_exponent -
												wild_cumulative_exponent;

				printf("x : %llu\n", computed_x);
				break;
			}

			hi = NULL;
		}

		jump_count++;
	}
}