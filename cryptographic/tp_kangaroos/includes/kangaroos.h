#ifndef __KANGAROOS___
#define __KANGAROOS___

#include <stdbool.h>
#include "mul11585.h"

num128 fast_exp(uint64_t g, uint64_t x);
num128 gexp(uint64_t x);
int test_gexp(void);

bool is_equal(num128 result, num128 expected);

num128 dlog64(num128 target);

#endif