#ifndef __SBOX__H_
#define __SBOX__H_

#include <stdint.h>

extern void random_sbox(uint8_t Sbox[256]);
extern void inverse_sbox(const uint8_t Sbox[256], uint8_t Sbox_inv[256]);

#endif
