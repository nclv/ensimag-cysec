#ifndef __UTILS__H_
#define __UTILS__H_

#include <stdint.h>

#include "aes-128_enc.h"

extern void print_array(uint8_t array[16]);
extern void xor_array(uint8_t l[16], uint8_t r[16], uint8_t result[16]);
extern void print_array2d(uint8_t array[256][16]);
extern int random_byte(uint8_t *byte);
extern int random_key(uint8_t key[16]);
extern bool is_equals(uint8_t block1[AES_BLOCK_SIZE], uint8_t block2[AES_BLOCK_SIZE]);

#endif
