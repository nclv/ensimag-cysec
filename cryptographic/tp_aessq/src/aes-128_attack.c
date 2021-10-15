#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "aes-128_attack.h"
#include "aes-128_enc.h"
#include "utils.h"

/*
 * Generate a random lambda set into @lambda_set.
 * ie. 256 random plaintexts of 16 bytes length with one byte taking all
 * possible values.
 */
int build_random_lambda_set(
	uint8_t lambda_set[AES_LAMBDA_SET_SIZE][AES_BLOCK_SIZE]) {
	// Get a random byte
	uint8_t c;
	int err = random_byte(&c);
	if (err == -1) {
		return -1;
	}

	// Create the initialization vector
	uint8_t init_vector[AES_BLOCK_SIZE] = {c, c, c, c, c, c, c, c,
										   c, c, c, c, c, c, c, c};
	// Set the first byte of each row to i, the others to c
	for (size_t i = 0; i < AES_LAMBDA_SET_SIZE; i++) {
		for (size_t j = 0; j < AES_BLOCK_SIZE; ++j) {
			lambda_set[i][j] = init_vector[j];
		}
		lambda_set[i][0] = i;
	}

	return 0;
}

uint8_t byte_reverse_add_round_key(uint8_t block_byte, uint8_t key_byte) {
	return block_byte ^ key_byte;
}

uint8_t byte_reverse_sub_bytes(uint8_t block_byte) { return Sinv[block_byte]; }

/*
 * partial_decrypt reverse AddRoundKey and SubBytes
 * We don't need to reverse ShiftRow.
 */
uint8_t partial_decrypt(uint8_t block_byte, uint8_t key_byte) {
	return byte_reverse_sub_bytes(
		byte_reverse_add_round_key(block_byte, key_byte));
}

/*
 * Returns true if the distinguisher condition is verified.
*/
bool distinguisher(uint8_t lambda_set[AES_LAMBDA_SET_SIZE][AES_BLOCK_SIZE],
				   size_t key_byte_index, uint8_t guessed_key_byte) {
	// sum holds the xored values of the partially decrypted lambda set
	uint8_t sum = 0;
	for (size_t i = 0; i < AES_LAMBDA_SET_SIZE; ++i) {
		sum ^= partial_decrypt(lambda_set[i][key_byte_index], guessed_key_byte);
	}

	return (sum == 0);
}

/*
 * Find the most common occurrence between all lambda sets.
 * @key_byte_counter holds the occurences of possible guessed key value for the
 * choosen key byte index
 * @returns true if there is only one most common occurrence
 */
bool most_common(size_t key_byte_counter[AES_KEY_BYTES_SIZE],
				 uint8_t *guessed_key_byte) {
	size_t max = 0;
	bool max_unique = true;
	uint8_t key_byte_count;
	for (uint16_t key_byte = 0; key_byte < AES_KEY_BYTES_SIZE; ++key_byte) {
		key_byte_count = key_byte_counter[key_byte];

		if (key_byte_count > max) {
			// New most common key byte
			max = key_byte_count;
			*guessed_key_byte = ((uint8_t)key_byte);
			max_unique = true;
		} else if (key_byte_count == max) {
			// There are 2 key bytes with the same occurence
			// count
			max_unique = false;
		}
	}

	return max_unique;
}

int aes128_attack(void) {
	// Generate a random key
	uint8_t key[AES_128_KEY_SIZE] = {0};

	int err = random_key(key);
	if (err == -1) {
		return -1;
	}

	// Decoded key after the attack
	uint8_t decoded_key[AES_128_KEY_SIZE] = {0};
	// Lambda set
	uint8_t lambda_set[AES_LAMBDA_SET_SIZE][AES_BLOCK_SIZE] = {{0}};
	// Counts the occurence of possible key bytes for all key bytes index
	// It is shared accross lambda sets.
	size_t key_bytes_counter[AES_128_KEY_SIZE][AES_KEY_BYTES_SIZE] = {{0}};
	// Counts the number of possible keys for a given byte
	size_t possible_key_byte_count[AES_128_KEY_SIZE] = {0};

	// counts the number of possible key byte guesses for a lambda set
	size_t key_byte_count;
	// holds the last possible key byte guess. When key_byte_count is equals to
	// 1, it holds the only possible key byte guess ie. the correct key byte.
	uint8_t guessed_key_byte;

	// counts the number of key bytes correctly guessed, it is incremented when
	// key_byte_count is equals to 1 for the current lambda_set or if we find an
	// unique maximum
	size_t key_bytes_guessed = 0;
	while (key_bytes_guessed < AES_128_KEY_SIZE) {
		// Generate a random lambda set
		err = build_random_lambda_set(lambda_set);

		// Encode the lambda set
		for (size_t i = 0; i < AES_LAMBDA_SET_SIZE; ++i) {
			aes128_enc(lambda_set[i], key, 4, 0); // encode 3 1/2 rounds
		}

		// Loop through the key bytes we try to guess
		for (size_t key_byte_index = 0; key_byte_index < AES_128_KEY_SIZE;
			 ++key_byte_index) {
			if (possible_key_byte_count[key_byte_index] == 1) {
				// The key byte was already found
				continue;
			}

			// (Re-)Initialize the count of key byte guesses for the current key
			// byte
			key_byte_count = 0;
			for (uint16_t key_byte = 0; key_byte < AES_KEY_BYTES_SIZE;
				 ++key_byte) {
				if (distinguisher(lambda_set, key_byte_index, key_byte)) {
					printf("Possible guess for byte %zu : %x \n",
						   key_byte_index, key_byte);
					// Increment the possible guesses counter for the next
					// iteration with a new lambda set
					key_bytes_counter[key_byte_index][key_byte]++;
					// Increment the possible guesses counter for the current
					// lambda set
					key_byte_count++;
					// Save the last guessed byte, used if there is only one
					// guess for this key byte
					guessed_key_byte = key_byte;
				}
			}

			printf("Possible keys count : %zu \n", key_byte_count);
			if (key_byte_count == 1) {
				// There is only one guessed key byte, it is the correct key
				// byte
				decoded_key[key_byte_index] = guessed_key_byte;
				key_bytes_guessed++;
			} else if (possible_key_byte_count[key_byte_index] > 0) {
				// There are many key bytes guesses and we aren't using the first lambda set

				if (most_common(key_bytes_counter[key_byte_index],
								&guessed_key_byte)) {
					// There is only one most common occurrence, we found the
					// correct key byte
					decoded_key[key_byte_index] = guessed_key_byte;
					key_byte_count = 1;
					key_bytes_guessed++;
				}
			}

			possible_key_byte_count[key_byte_index] = key_byte_count;
		}

		printf("Number of key bytes guessed : %zu\n", key_bytes_guessed);
        printf("We have yet to find %zu key bytes.\n", AES_128_KEY_SIZE - key_bytes_guessed);
	}

	printf("Key : \n");
	print_array(key);

	uint8_t tmp[AES_128_KEY_SIZE];
	uint8_t round3_key[AES_128_KEY_SIZE];

	printf("3rd round key : \n");
	next_aes128_round_key(key, tmp, 0);
	next_aes128_round_key(tmp, round3_key, 1);
	next_aes128_round_key(round3_key, tmp, 2);
	next_aes128_round_key(tmp, round3_key, 3);
	print_array(round3_key);

	printf("3rd round decoded key : \n");
	print_array(decoded_key);

	printf("3rd round key and decoded 3rd round key are equals : %s\n",
		   is_equals(decoded_key, round3_key) ? "true" : "false");

	prev_aes128_round_key(decoded_key, tmp, 3);
	prev_aes128_round_key(tmp, decoded_key, 2);
	prev_aes128_round_key(decoded_key, tmp, 1);
	prev_aes128_round_key(tmp, decoded_key, 0);

	printf("Decoded key : \n");
	print_array(decoded_key);

	printf("Key and decoded key are equals : %s\n",
		   is_equals(decoded_key, key) ? "true" : "false");

	return 0;
}