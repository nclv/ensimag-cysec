#include <stdint.h>

#include "aes-128_enc.h"

#define AES_LAMBDA_SET_SIZE 256
#define AES_KEY_BYTES_SIZE 256

#ifndef __AES128_ATTACK__H_
#define __AES128_ATTACK__H_

extern int build_random_lambda_set(
	uint8_t lambda_set[AES_LAMBDA_SET_SIZE][AES_BLOCK_SIZE]);
extern int aes128_attack(uint8_t (*xtime)(uint8_t));

#endif
