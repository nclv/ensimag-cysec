#ifndef __KANGAROOS___
#define __KANGAROOS___

#include "mul11585.h"
#include <stdbool.h>

num128 fast_exp(uint64_t g, uint64_t x);
num128 gexp(uint64_t x);
bool test_gexp(void);
bool is_equal(num128 result, num128 expected);

void fill_exponents(uint64_t *ej, num128 *gexpej);
bool test_fill_exponents(void);
uint64_t get_exponent_for_subset(uint64_t *ej, num128 *gexpej, num128 x, num128 *gej);
bool is_distinguished(num128 x);

num128 dlog64(num128 target);
bool test_dlog64(void);

#endif