
#ifndef __UTILS___
#define __UTILS___

#include <stdint.h>
#include <stddef.h>

extern int assert_equals(const void *actual, const void *expected, size_t actual_size, size_t expected_size);

extern void print_array(const uint32_t *array, size_t size);

extern int random_m(uint32_t m[4]);
extern void random_message(uint32_t m[4]);

#endif
