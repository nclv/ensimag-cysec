
#include <stdint.h>
#include <stdlib.h>

/*
 * inverse_sbox returns the inverse S-box.
 */
void inverse_sbox(const uint8_t Sbox[256], uint8_t Sbox_inv[256]) {
	for (uint16_t i = 0; i < 256; ++i) {
		Sbox_inv[Sbox[i]] = i;
	}
}

/*
 * swap 2 uint8_t
 */
void swap(uint8_t *a, uint8_t *b) {
	uint8_t temp = *a;
	*a = *b;
	*b = temp;
}

/*
 * shuffle a S-box
 */
void shuffle(uint8_t Sbox[256]) {
	for (int i = 255; i > 0; --i) {
		int j = rand() % (i + 1);
		swap(&Sbox[i], &Sbox[j]);
	}
}

/*
 * random_sbox returns a random S-box.
 */
void random_sbox(uint8_t Sbox[256]) {
	for (uint16_t i = 0; i < 256; ++i) {
		Sbox[i] = i;
	}
	// shuffle the s-box
	shuffle(Sbox);
}
