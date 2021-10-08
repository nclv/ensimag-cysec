/*
 * AES-128 Encryption
 * Byte-Oriented
 * On-the-fly key schedule
 * Constant-time XTIME
 */

#include "aes-128_enc.h"
#include "utils.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Constant-time ``broadcast-based'' multiplication by $a$ in $F_2[X]/X^8 + X^4
 * + X^3 + X + 1$
 */
uint8_t xtime(uint8_t p) {
	uint8_t m = p >> 7;

	m ^= 1;
	m -= 1;
	m &= 0x1B; // 0x7B for X8 + X6 + X5 + X4 + X3 + X + 1

	return ((p << 1) ^ m);
}

/*
 * The round constants
 */
static const uint8_t RC[10] = {0x01, 0x02, 0x04, 0x08, 0x10,
							   0x20, 0x40, 0x80, 0x1B, 0x36};

void aes_round(uint8_t block[AES_BLOCK_SIZE], uint8_t round_key[AES_BLOCK_SIZE],
			   int lastround) {
	int i;
	uint8_t tmp;

	/*
	 * SubBytes + ShiftRow
	 */
	/* Row 0 */
	block[0] = S[block[0]];
	block[4] = S[block[4]];
	block[8] = S[block[8]];
	block[12] = S[block[12]];
	/* Row 1 */
	tmp = block[1];
	block[1] = S[block[5]];
	block[5] = S[block[9]];
	block[9] = S[block[13]];
	block[13] = S[tmp];
	/* Row 2 */
	tmp = block[2];
	block[2] = S[block[10]];
	block[10] = S[tmp];
	tmp = block[6];
	block[6] = S[block[14]];
	block[14] = S[tmp];
	/* Row 3 */
	tmp = block[15];
	block[15] = S[block[11]];
	block[11] = S[block[7]];
	block[7] = S[block[3]];
	block[3] = S[tmp];

	/*
	 * MixColumns
	 */
	for (i = lastround; i < 16;
		 i += 4) /* lastround = 16 if it is the last round, 0 otherwise */
	{
		uint8_t *column = block + i;
		uint8_t tmp2 = column[0];
		tmp = column[0] ^ column[1] ^ column[2] ^ column[3];

		column[0] ^= tmp ^ xtime(column[0] ^ column[1]);
		column[1] ^= tmp ^ xtime(column[1] ^ column[2]);
		column[2] ^= tmp ^ xtime(column[2] ^ column[3]);
		column[3] ^= tmp ^ xtime(column[3] ^ tmp2);
	}

	/*
	 * AddRoundKey
	 */
	for (i = 0; i < 16; i++) {
		block[i] ^= round_key[i];
	}
}

/*
 * Compute the @(round + 1)-th round key in @next_key, given the @round-th key
 * in @prev_key
 * @round in {0...9}
 * The ``master key'' is the 0-th round key
 */
void next_aes128_round_key(const uint8_t prev_key[16], uint8_t next_key[16],
						   int round) {
	int i;

	// print_array(next_key);
	next_key[0] = prev_key[0] ^ S[prev_key[13]] ^ RC[round];
	// print_array(next_key);
	next_key[1] = prev_key[1] ^ S[prev_key[14]];
	// print_array(next_key);
	next_key[2] = prev_key[2] ^ S[prev_key[15]];
	// print_array(next_key);
	next_key[3] = prev_key[3] ^ S[prev_key[12]];

	for (i = 4; i < 16; i++) {
		// print_array(next_key);
		next_key[i] = prev_key[i] ^ next_key[i - 4];
	}
}

/*
 * Compute the @round-th round key in @prev_key, given the @(round + 1)-th key
 * in @next_key
 * @round in {0...9}
 * The ``master decryption key'' is the 10-th round key (for a full AES-128)
 */
void prev_aes128_round_key(const uint8_t next_key[16], uint8_t prev_key[16],
						   int round) {
	int i;

	for (i = 4; i < 16; i++) {
		// print_array(prev_key);
		prev_key[i] = next_key[i] ^ next_key[i - 4];
	}

	//   print_array(prev_key);
	prev_key[0] = next_key[0] ^ S[prev_key[13]] ^ RC[round];
	//   print_array(prev_key);
	prev_key[1] = next_key[1] ^ S[prev_key[14]];
	//   print_array(prev_key);
	prev_key[2] = next_key[2] ^ S[prev_key[15]];
	//   print_array(prev_key);
	prev_key[3] = next_key[3] ^ S[prev_key[12]];
}

/*
 * Encrypt @block with @key over @nrounds. If @lastfull is true, the last round
 * includes MixColumn, otherwise it doesn't.
 * @nrounds <= 10
 */
void aes128_enc(uint8_t block[AES_BLOCK_SIZE],
				const uint8_t key[AES_128_KEY_SIZE], unsigned nrounds,
				int lastfull) {
	uint8_t ekey[32];
	int i, pk, nk;

	for (i = 0; i < 16; i++) {
		block[i] ^= key[i];
		ekey[i] = key[i];
	}
	next_aes128_round_key(ekey, ekey + 16, 0);

	pk = 0;
	nk = 16;
	for (i = 1; i < nrounds; i++) {
		aes_round(block, ekey + nk, 0);
		pk = (pk + 16) & 0x10;
		nk = (nk + 16) & 0x10;
		next_aes128_round_key(ekey + pk, ekey + nk, i);
	}
	if (lastfull) {
		aes_round(block, ekey + nk, 0);
	} else {
		aes_round(block, ekey + nk, 16);
	}
}

int main(int argc, char **argv) {
	uint8_t mult = xtime(3);
	printf("%d\n", mult);

	const uint8_t prev_key[AES_128_KEY_SIZE] = {
		0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6,
		0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c};
	uint8_t next_key[AES_128_KEY_SIZE];
	print_array(prev_key);

	// prev_key is the master key because round = 0
	next_aes128_round_key(prev_key, next_key, 0);
	print_array(next_key);

	uint8_t prev_key_computed[AES_128_KEY_SIZE];

	printf("Inverse\n");
	prev_aes128_round_key(next_key, prev_key_computed, 0);
	print_array(prev_key_computed);

	// AES 3 1/2 rounds
	uint8_t block[AES_BLOCK_SIZE] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a,
									 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2,
									 0xe0, 0x37, 0x07, 0x34};
	const uint8_t key[AES_128_KEY_SIZE] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae,
										   0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88,
										   0x09, 0xcf, 0x4f, 0x3c};
	printf("Block and key (before) :\n");
	print_array(block);
	print_array(key);

	aes128_enc(block, key, 10, 0);

	printf("Block and key (after):\n");
	print_array(block);
	print_array(key);

	printf("Xored (Q3):\n");

	const uint8_t key1[AES_128_KEY_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x05,
											0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
											0x0c, 0x0d, 0x0e, 0x0f};
	const uint8_t key2[AES_128_KEY_SIZE] = {0x11, 0x12, 0x13, 0x14, 0x15, 0x15,
											0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
											0x1c, 0x1d, 0x1e, 0x1f};

	uint8_t plaintext[AES_BLOCK_SIZE] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
										 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,
										 0xcc, 0xdd, 0xee, 0xff};

	uint8_t enc1[AES_BLOCK_SIZE];
	memcpy(enc1, plaintext, sizeof(plaintext));
	print_array(enc1);
	aes128_enc(enc1, key1, 3, 1);
	print_array(enc1);

	print_array(plaintext);
	aes128_enc(plaintext, key1, 3, 1); // same key -> xored = 0
	print_array(plaintext);

	uint8_t xored[AES_BLOCK_SIZE];
	xor_array(enc1, plaintext, xored);
	print_array(xored);

	// Generate the lambda set
	uint8_t lambda[AES_BLOCK_SIZE * AES_BLOCK_SIZE][AES_BLOCK_SIZE] = {{0}};
	uint8_t c = 5;
	uint8_t dumb[AES_BLOCK_SIZE] = {c, c, c, c, c, c, c, c, c, c, c, c, c, c, c, c};
	for (size_t i = 0; i < AES_BLOCK_SIZE*AES_BLOCK_SIZE; i++) {
		for (size_t j = 0; j < AES_BLOCK_SIZE; ++j) {
			lambda[i][j] = dumb[j];
		}
		lambda[i][0] = i;
	}

	printf("Lambda :\n");
	print_array2d(lambda);
}