#ifndef __LAMBDA___
#define __LAMBDA___

#include <stdint.h>

#include "mul11585.h"

unsigned long long next_jump_size(num128 *x, uint8_t k);
void generate_powers_of_two(uint64_t *powers_of_two, uint8_t max);
void lambda_method(void);
void lambda_method_simultaneous(void);

#endif