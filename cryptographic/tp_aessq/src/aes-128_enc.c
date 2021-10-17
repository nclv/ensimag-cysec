/*
 * AES-128 Encryption
 * Byte-Oriented
 * On-the-fly key schedule
 * Constant-time XTIME
 */

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "aes-128_attack.h"
#include "aes-128_enc.h"
#include "sbox.h"
#include "utils.h"

/*
 * Constant-time ``broadcast-based'' multiplication by $a$ in $F_2[X]/X^8 + X^4
 * + X^3 + X + 1$
 */
uint8_t xtime(uint8_t p) {
	uint8_t m = p >> 7;

	m ^= 1;
	m -= 1;
	m &= 0x1B;

	return (uint8_t)((p << 1) ^ m);
}

/*
 * Constant-time ``broadcast-based'' multiplication by $a$ in $F_2[X]/X^8 + X^6
 * + X^5 + X^4 + X^3 + X + 1$
 */
uint8_t xtime_variant(uint8_t p) {
	uint8_t m = p >> 7;

	m ^= 1;
	m -= 1;
	m &= 0x7B;

	return (uint8_t)((p << 1) ^ m);
}

/*
 * The round constants
 */
static const uint8_t RC[10] = {0x01, 0x02, 0x04, 0x08, 0x10,
							   0x20, 0x40, 0x80, 0x1B, 0x36};

/*
 * aes_round encode @block with @round_key.
 * If lastround is 16, MixColumns operation is not done.
 */
void aes_round(uint8_t block[AES_BLOCK_SIZE], uint8_t round_key[AES_BLOCK_SIZE],
			   int lastround, uint8_t (*xtime)(uint8_t), const uint8_t Sbox[256]) {
	int i;
	uint8_t tmp;

	/*
	 * SubBytes + ShiftRow
	 */
	/* Row 0 */
	block[0] = Sbox[block[0]];
	block[4] = Sbox[block[4]];
	block[8] = Sbox[block[8]];
	block[12] = Sbox[block[12]];
	/* Row 1 */
	tmp = block[1];
	block[1] = Sbox[block[5]];
	block[5] = Sbox[block[9]];
	block[9] = Sbox[block[13]];
	block[13] = Sbox[tmp];
	/* Row 2 */
	tmp = block[2];
	block[2] = Sbox[block[10]];
	block[10] = Sbox[tmp];
	tmp = block[6];
	block[6] = Sbox[block[14]];
	block[14] = Sbox[tmp];
	/* Row 3 */
	tmp = block[15];
	block[15] = Sbox[block[11]];
	block[11] = Sbox[block[7]];
	block[7] = Sbox[block[3]];
	block[3] = Sbox[tmp];

	/*
	 * MixColumns
	 */
	for (i = lastround; i < 16;
		 i += 4) /* lastround = 16 if it is the last round, 0 otherwise */
	{
		uint8_t *column = block + i;
		uint8_t tmp2 = column[0];
		tmp = column[0] ^ column[1] ^ column[2] ^ column[3];

		column[0] ^= tmp ^ (*xtime)(column[0] ^ column[1]);
		column[1] ^= tmp ^ (*xtime)(column[1] ^ column[2]);
		column[2] ^= tmp ^ (*xtime)(column[2] ^ column[3]);
		column[3] ^= tmp ^ (*xtime)(column[3] ^ tmp2);
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
						   int round, const uint8_t Sbox[256]) {
	int i;

	// print_array(next_key);
	next_key[0] = prev_key[0] ^ Sbox[prev_key[13]] ^ RC[round];
	// print_array(next_key);
	next_key[1] = prev_key[1] ^ Sbox[prev_key[14]];
	// print_array(next_key);
	next_key[2] = prev_key[2] ^ Sbox[prev_key[15]];
	// print_array(next_key);
	next_key[3] = prev_key[3] ^ Sbox[prev_key[12]];

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
						   int round, const uint8_t Sbox[256]) {
	int i;

	for (i = 4; i < 16; i++) {
		// print_array(prev_key);
		prev_key[i] = next_key[i] ^ next_key[i - 4];
	}

	//   print_array(prev_key);
	prev_key[0] = next_key[0] ^ Sbox[prev_key[13]] ^ RC[round];
	//   print_array(prev_key);
	prev_key[1] = next_key[1] ^ Sbox[prev_key[14]];
	//   print_array(prev_key);
	prev_key[2] = next_key[2] ^ Sbox[prev_key[15]];
	//   print_array(prev_key);
	prev_key[3] = next_key[3] ^ Sbox[prev_key[12]];
}

/*
 * Encrypt @block with @key over @nrounds. If @lastfull is true, the last round
 * includes MixColumn, otherwise it doesn't.
 * @nrounds <= 10
 */
void aes128_enc(uint8_t block[AES_BLOCK_SIZE],
				const uint8_t key[AES_128_KEY_SIZE], unsigned nrounds,
				int lastfull, uint8_t (*xtime)(uint8_t), const uint8_t Sbox[256]) {
	uint8_t ekey[32];
	int i, pk, nk;

	// AddRoundKey
	for (i = 0; i < 16; i++) {
		block[i] ^= key[i];
		ekey[i] = key[i];
	}
	next_aes128_round_key(ekey, ekey + 16, 0, Sbox);

	pk = 0;
	nk = 16;
	for (i = 1; i < nrounds; i++) {
		aes_round(block, ekey + nk, 0, xtime, Sbox);
		pk = (pk + 16) & 0x10;
		nk = (nk + 16) & 0x10;
		next_aes128_round_key(ekey + pk, ekey + nk, i, Sbox);
	}
	if (lastfull) {
		aes_round(block, ekey + nk, 0, xtime, Sbox);
	} else {
		aes_round(block, ekey + nk, 16, xtime, Sbox);
	}
}

void question1(void) {
	printf("-- Question 1 : Verification of the xtime function --\n");
	// Test with P = X + 1
	uint8_t mult = xtime(3);
	printf("Expected result : 0x6. Result : 0x%x.\n", mult);

	// Test with P = X^7 + X^4 + X^3 + X + 1
	mult = xtime(0x9B);
	printf("Expected result : 0x2D. Result : 0x%x.\n", mult);
	printf("\n");
}

void question2(void) {
	printf("-- Question 2 : Correctness of the function prev_aes128_round_key "
		   "--\n");

	const uint8_t prev_key[AES_128_KEY_SIZE] = {
		0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab,
		0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c}; // key values provided in the
												   // standard document
	uint8_t next_key[AES_128_KEY_SIZE];

	printf("Test for a round on the key:\n");
	printf("Original key\n");
	print_array(prev_key);

	// prev_key is the master key because round = 0
	printf("Next key\n");
	next_aes128_round_key(prev_key, next_key, 0, S);
	print_array(next_key);

	uint8_t prev_key_computed[AES_128_KEY_SIZE];

	printf("Prev key\n");
	prev_aes128_round_key(next_key, prev_key_computed, 0, S);
	print_array(prev_key_computed);

	printf("\n");
}

/*
 * f_construction build a keyed function F from a block cipher E as E(key1,
 * plaintext) ⊕ E(key2, plaintext) The result is stored in xored.
 */
void f_construction(const uint8_t key1[AES_128_KEY_SIZE],
					const uint8_t key2[AES_128_KEY_SIZE],
					uint8_t plaintext[AES_BLOCK_SIZE],
					uint8_t xored[AES_BLOCK_SIZE]) {

	uint8_t enc1[AES_BLOCK_SIZE], enc2[AES_BLOCK_SIZE];

	memcpy(enc1, plaintext, sizeof(uint8_t) * AES_BLOCK_SIZE);
	aes128_enc(enc1, key1, 3, 1, xtime, S);
	// printf("Encryption: E(k_1, x)=\n");
	// print_array(enc1);

	memcpy(enc2, plaintext, sizeof(uint8_t) * AES_BLOCK_SIZE);
	aes128_enc(enc2, key2, 3, 1, xtime, S); // same key -> xored = 0
	// printf("Encryption: E(k_2, x)=\n");
	// print_array(enc2);

	xor_array(enc1, enc2, xored);
	// printf("F(k_1||k_2, x)=\n");
	// print_array(xored);
}

void question3(void) {
	printf("-- Question 3 : Implementation of the key function F (xored of "
		   "E(k_1, x) and E(k_2, x)) --\n");

	const uint8_t key1[AES_128_KEY_SIZE] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x05,
											0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b,
											0x0c, 0x0d, 0x0e, 0x0f};
	const uint8_t key2[AES_128_KEY_SIZE] = {0x11, 0x12, 0x13, 0x14, 0x15, 0x15,
											0x16, 0x17, 0x18, 0x19, 0x1a, 0x1b,
											0x1c, 0x1d, 0x1e, 0x1f};

	uint8_t plaintext[AES_BLOCK_SIZE] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
										 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb,
										 0xcc, 0xdd, 0xee, 0xff};
	uint8_t xored[AES_BLOCK_SIZE];

	printf("Test with k_1=k_2\n");
	printf("Block and keys (before) :\n");
	print_array(plaintext);
	print_array(key1);
	print_array(key1);

	f_construction(key1, key1, plaintext, xored);
	printf("F(k_1||k_2, x)=\n");
	print_array(xored);
	printf("\n");

	printf("Test with k_1 != k_2 : Verification than the sum for each "
		   "coordinates of all a lambda set is the vector 0.\n");
	printf("Keys (before) :\n");
	print_array(key1);
	print_array(key2);

	uint8_t lambda_set[AES_LAMBDA_SET_SIZE][AES_BLOCK_SIZE];
	uint8_t enc_lambda_set[AES_LAMBDA_SET_SIZE][AES_BLOCK_SIZE];
	uint8_t sum_coordinates[AES_BLOCK_SIZE] = {0};

	build_random_lambda_set(lambda_set);

	for (size_t i = 0; i < AES_LAMBDA_SET_SIZE; i++) {
		f_construction(key1, key2, lambda_set[i], enc_lambda_set[i]);

		for (size_t coordinate = 0; coordinate < AES_BLOCK_SIZE; coordinate++) {
			sum_coordinates[coordinate] ^= enc_lambda_set[i][coordinate];
		}
	}

	printf("Verification of the distinguisher's property (xor coordinates for "
		   "a lambda set is the vector 0):\n");
	print_array(sum_coordinates);

	printf("\n");
}

/*
 * full_encryption performs 9¹/² rounds encryption of a block.
 */
void full_encryption(uint8_t block[AES_BLOCK_SIZE],
					 const uint8_t key[AES_128_KEY_SIZE],
					 uint8_t (*xtime)(uint8_t)) {
	uint8_t tmp[AES_BLOCK_SIZE];

	printf("Test for the encryption of a block with a key (9¹/² rounds):\n");
	printf("Block and key (before) :\n");
	memcpy(tmp, block, sizeof(uint8_t) * AES_BLOCK_SIZE);
	print_array(tmp);
	print_array(key);

	aes128_enc(tmp, key, 10, 0, xtime, S);

	printf("Block and key (after):\n");
	print_array(tmp);
	print_array(key);
	printf("\n\n");
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	// AES 3 10-rounds
	// Test with test values provided in the standard document
	uint8_t block[AES_BLOCK_SIZE] = {0x32, 0x43, 0xf6, 0xa8, 0x88, 0x5a,
									 0x30, 0x8d, 0x31, 0x31, 0x98, 0xa2,
									 0xe0, 0x37, 0x07, 0x34};
	const uint8_t key[AES_128_KEY_SIZE] = {0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae,
										   0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88,
										   0x09, 0xcf, 0x4f, 0x3c};

	full_encryption(block, key, xtime);

	printf("--- EXERCICE 1 : WARMING UP ---\n");
	question1();
	question2();
	question3();

	printf("--- EXERCICE 2 : KEY-RECOVERY ATTACK FOR 3¹/²-ROUND AES ---\n");
	aes128_attack(xtime, S, Sinv);

	printf("\n-- TEST : Check that the cipher is not the same with a different xtime "
		   "function.\n");
	printf("With xtime\n");
	full_encryption(block, key, xtime);
	printf("With xtime_variant\n");
	full_encryption(block, key, xtime_variant);

	printf("\n--TEST : Same attack with xtime_variant :\n");
	aes128_attack(xtime_variant, S, Sinv);

	printf("\n\n--TEST : Same attack with xtime and an other Sbox :\n");
	printf("(Note that a modification of xtime modify the MDS matrix.)\n");
	printf("Modifying S-box\n");
	// Create a random S-box
	uint8_t S_alt[256] = {0};
	random_sbox(S_alt);
	// Build the s-box inverse
	uint8_t S_alt_inv[256] = {0};
	inverse_sbox(S_alt, S_alt_inv);

	aes128_attack(xtime, S_alt, S_alt_inv);
}