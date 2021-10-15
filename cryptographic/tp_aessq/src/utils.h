#ifndef utils_H_
#define utils_H_

#include <stdint.h>

extern void print_array(uint8_t array[16]);
extern void xor_array(uint8_t l[16], uint8_t r[16], uint8_t result[16]);
extern void print_array2d(uint8_t array[256][16]);
extern int random_byte(uint8_t *byte);
extern int random_key(uint8_t key[16]);

#endif
