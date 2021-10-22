#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "utils.h"

// S^{-alpha} with alpha = 8 perform a circular shift of 8 bits to the right ie.
// 16 bits to the left mod 24 bits. Used in the speck48_96 encryption.
#define ROTL24_16(x) ((((x) << 16) ^ ((x) >> 8)) & 0xFFFFFF)
// S^beta with beta = 3 performs a circular shift of 3 bits to the left.
// Used in the speck48_96 encryption.
#define ROTL24_3(x) ((((x) << 3) ^ ((x) >> 21)) & 0xFFFFFF)
// Inverse operations in the decryption function : S^alpha and S^{-beta}
#define ROTL24_8(x) ((((x) << 8) ^ ((x) >> 16)) & 0xFFFFFF)
#define ROTL24_21(x) ((((x) << 21) ^ ((x) >> 3)) & 0xFFFFFF)

#define IV 0x010203040506ULL

/*
 * the 96-bit key is stored in four 24-bit chunks in the low bits of k[0]...k[3]
 * the 48-bit plaintext is stored in two 24-bit chunks in the low bits of p[0],
 * p[1] the 48-bit ciphertext is written similarly in c
 */
void speck48_96(const uint32_t k[4], const uint32_t p[2], uint32_t c[2]) {
	uint32_t rk[23];
	uint32_t ell[3] = {k[1], k[2], k[3]};

	rk[0] = k[0];

	// ciphertext output
	c[0] = p[0];
	c[1] = p[1];

	/* full key schedule */
	for (unsigned i = 0; i < 22; i++) {
		// l_{i+m-1} in the spec.
		uint32_t new_ell = ((ROTL24_16(ell[0]) + rk[i]) ^ i) &
						   0xFFFFFF; // addition (+) is done mod 2**24
		// k[i+1] in the spec.
		rk[i + 1] = ROTL24_3(rk[i]) ^ new_ell;
		ell[0] = ell[1];
		ell[1] = ell[2];
		ell[2] = new_ell;
	}

	// 23 rounds of encryption
	for (unsigned i = 0; i < 23; i++) {
		c[0] = ((ROTL24_16(c[0]) + c[1]) ^ rk[i]) & 0xFFFFFF;
		c[1] = ROTL24_3(c[1]) ^ c[0];
	}

	return;
}

/*
 * test the speck48_96 function
 */
int test_speck48_96(void) {
	// inversed order for the chunks of the key
	const uint32_t k[4] = { 
		0x020100,
		0x0a0908,
		0x121110,
		0x1a1918,
	};
	const uint32_t p[2] = {0x6d2073, 0x696874};
	uint32_t c[2] = {0};

	uint32_t expected[2] = {0x735e10, 0xb6445d};
	printf("Expected ciphertext :\n");
	print_array(expected, 2);

	speck48_96(k, p, c);
	printf("Ciphertext :\n");
	print_array(c, 2);

	return assert_equals(c, expected);
}

/* the inverse cipher */
void speck48_96_inv(const uint32_t k[4], const uint32_t c[2], uint32_t p[2]) {
	uint32_t rk[23];
	uint32_t ell[3] = {k[1], k[2], k[3]};

	rk[0] = k[0];

	// ciphertext output
	p[0] = c[0];
	p[1] = c[1];

	/* full key schedule */
	for (unsigned i = 0; i < 22; i++) {
		// l_{i+m-1} in the spec.
		uint32_t new_ell = ((ROTL24_16(ell[0]) + rk[i]) ^ i) &
						   0xFFFFFF; // addition (+) is done mod 2**24
		// k[i+1] in the spec.
		rk[i + 1] = ROTL24_3(rk[i]) ^ new_ell;
		ell[0] = ell[1];
		ell[1] = ell[2];
		ell[2] = new_ell;
	}

	// 23 rounds of decryption
	for (int i = 22; i >= 0; i--) {
		p[1] = ROTL24_21(p[1] ^ p[0]);
		p[0] = ROTL24_8(((p[0] ^ rk[i]) - p[1]) & 0xFFFFFF);
	}

	return;
}

/*
 * test the speck48_96_inv function
 */
int test_speck48_96_inv(void) {
	// inversed order for the chunks of the key
	const uint32_t k[4] = { 
		0x020100,
		0x0a0908,
		0x121110,
		0x1a1918,
	};
	const uint32_t p_original[2] = {0x6d2073, 0x696874};
	uint32_t c[2] = {0};
	speck48_96(k, p_original, c);
	uint32_t p[2] = {0};

	printf("Expected plaintext :\n");
	print_array(p_original, 2);

	speck48_96_inv(k, c, p);
	printf("Plaintext :\n");
	print_array(p, 2);

	return assert_equals(p, p_original);
}


/* The Davies-Meyer compression function based on speck48_96,
 * using an XOR feedforward
 * The input/output chaining value is given on the 48 low bits of a single
 * 64-bit word, whose 24 lower bits are set to the low half of the
 * "plaintext"/"ciphertext" (p[0]/c[0]) and whose 24 higher bits are set to the
 * high half (p[1]/c[1])
 */
uint64_t cs48_dm(const uint32_t m[4], const uint64_t h) { /* FILL ME */
	const uint32_t p[2] = {h & 0xFFFFFF, (h >> 24) & 0xFFFFFF};
	uint32_t c[2] = {0};
	speck48_96(m, p, c);

	return (((uint64_t) c[1]) << 24 | ((uint64_t) c[0])) ^ h;
}

/*
 * test the cs48_dm function
 */
int test_cs48_dm(void) {
	// inversed order for the chunks of the key
	const uint32_t k[4] = { 
		0x020100,
		0x0a0908,
		0x121110,
		0x1a1918,
	};
	const uint32_t m[4] = {0, 0, 0, 0};

	const uint64_t expected = 0x7FDD5A6EB248ULL;
	printf("Expected result of the compression function :\n");
	printf("%llx\n", expected);

	uint64_t c = cs48_dm(m, 0);
	printf("Result :\n");
	printf("%llx\n", c);

	return (c == expected) ? 0 : -1;
}

/* assumes message length is fourlen * four blocks of 24 bits, each stored as
 * the low bits of 32-bit words fourlen is stored on 48 bits (as the 48 low bits
 * of a 64-bit word)
 * when padding is include, simply adds one block (96 bits) of padding with
 * fourlen and zeros on higher pos */
uint64_t hs48(const uint32_t *m, uint64_t fourlen, int padding, int verbose) {
	uint64_t h = IV;
	const uint32_t *mp = m;

	for (uint64_t i = 0; i < fourlen; i++) {
		h = cs48_dm(mp, h);
		if (verbose)
			printf("@%llu : %06X %06X %06X %06X => %06llX\n", i, mp[0], mp[1],
				   mp[2], mp[3], h);
		mp += 4;
	}
	if (padding) {
		uint32_t pad[4];
		pad[0] = fourlen & 0xFFFFFF;
		pad[1] = (fourlen >> 24) & 0xFFFFFF;
		pad[2] = 0;
		pad[3] = 0;
		h = cs48_dm(pad, h);
		if (verbose)
			printf("@%llu : %06X %06X %06X %06X => %06llX\n", fourlen, pad[0],
				   pad[1], pad[2], pad[3], h);
	}

	return h;
}

/* Computes the unique fixed-point for cs48_dm for the message m */
uint64_t get_cs48_dm_fp(uint32_t m[4]) { /* FILL ME */
	// cs48_dm(m, fp) = fp means E(m, fp) ^ fp = fp.
	// So, we have E(m, fp) = 0, i.e. E^-1(m, 0) = fp

	const uint32_t c[2] = {0, 0};
	uint32_t fp[2] = {0};
	speck48_96_inv(m, c, fp);
	return (((uint64_t) fp[1]) << 24 | ((uint64_t) fp[0]));
}

/*
 * test the cs48_dm_fp function
 */
int test_cs48_dm_fp(void) {
	uint32_t m[4] = {0};
	random_m(m);
	printf("Message: ");
	print_array(m, 4);
	uint64_t fp = get_cs48_dm_fp(m);
	uint64_t expected = cs48_dm(m, fp);
	
	return expected == fp ? 0 : -1;
}

/* Finds a two-block expandable message for hs48, using a fixed-point
 * That is, computes m1, m2 s.t. hs48_nopad(m1||m2) = hs48_nopad(m1||m2^*),
 * where hs48_nopad is hs48 with no padding */
void find_exp_mess(uint32_t m1[4], uint32_t m2[4]) { /* FILL ME */
}

void attack(void) { /* FILL ME */
}

void part1() {
	printf("---Part one: preparatory work---\n");
	int err = test_speck48_96();
	printf("Test speck48_96 is correct: %s\n\n", err == 0 ? "true" : "false");

	err = test_speck48_96_inv();
	printf("Test speck48_96_inv is correct: %s\n\n", err == 0 ? "true" : "false");
	
	err = test_cs48_dm();
	printf("Test cs48_dm is correct: %s\n\n", err == 0 ? "true" : "false");

	err = test_cs48_dm_fp();
	printf("Test get_cs48_dm_fp is correct: %s\n\n", err == 0 ? "true" : "false");
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	part1();

	attack();

	return 0;
}
