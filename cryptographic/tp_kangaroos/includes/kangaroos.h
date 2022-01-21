#ifndef __KANGAROOS___
#define __KANGAROOS___

#include "mul11585.h"

#include <stdbool.h>

num128 fast_exp(uint64_t g, uint64_t x);
num128 gexp(uint64_t x);
bool test_gexp(void);

void generate_powers_of_two_exponents(uint64_t *exponents,
									  num128 *g_power_exponents);
bool is_distinguished(num128 x);

num128 dlog64(num128 target);
bool test_dlog64(void);

#endif