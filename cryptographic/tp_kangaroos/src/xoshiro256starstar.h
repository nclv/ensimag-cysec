/*  Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)

	To the extent possible under law, the author has dedicated all copyright
	and related and neighboring rights to this software to the public domain
	worldwide. This software is distributed without any warranty.

	See <http://creativecommons.org/publicdomain/zero/1.0/>. */

/* basic wrappers for convenience, PK, 2018 */

#ifndef __XOSHIRO256starstar___
#define __XOSHIRO256starstar___


#include <stdint.h>

uint64_t xoshiro256starstar_random(void);

uint64_t xoshiro256starstar_random_unsafe(void);

void xoshiro256starstar_random_set(uint64_t seed[4]);

#endif
