
#ifndef __UTILS___
#define __UTILS___

#include <stdint.h>
#include <stddef.h>

int assert_equals(uint32_t result[2], uint32_t expected[2]);
void print_array(uint32_t *array, size_t size);
int random_m(uint32_t m[4]);
void random_message(uint32_t m[4]);

#endif
