#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ROTL24_16(x) ((((x) << 16) ^ ((x) >> 8)) & 0xFFFFFF)
#define ROTL24_3(x) ((((x) << 3) ^ ((x) >> 21)) & 0xFFFFFF)

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

	c[0] = p[0];
	c[1] = p[1];

	/* full key schedule */
	for (unsigned i = 0; i < 22; i++) {
		uint32_t new_ell = ((ROTL24_16(ell[0]) + rk[i]) ^ i) &
						   0xFFFFFF; // addition (+) is done mod 2**24
		rk[i + 1] = ROTL24_3(rk[i]) ^ new_ell;
		ell[0] = ell[1];
		ell[1] = ell[2];
		ell[2] = new_ell;
	}

	for (unsigned i = 0; i < 23; i++) {
		/* FILL ME */
	}

	return;
}

/* the inverse cipher */
void speck48_96_inv(const uint32_t k[4], const uint32_t c[2], uint32_t p[2]) {
	/* FILL ME */
}

/* The Davies-Meyer compression function based on speck48_96,
 * using an XOR feedforward
 * The input/output chaining value is given on the 48 low bits of a single
 * 64-bit word, whose 24 lower bits are set to the low half of the
 * "plaintext"/"ciphertext" (p[0]/c[0]) and whose 24 higher bits are set to the
 * high half (p[1]/c[1])
 */
uint64_t cs48_dm(const uint32_t m[4], const uint64_t h) { /* FILL ME */
	return 0;
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
	return 0;
}

/* Finds a two-block expandable message for hs48, using a fixed-point
 * That is, computes m1, m2 s.t. hs48_nopad(m1||m2) = hs48_nopad(m1||m2^*),
 * where hs48_nopad is hs48 with no padding */
void find_exp_mess(uint32_t m1[4], uint32_t m2[4]) { /* FILL ME */
}

void attack(void) { /* FILL ME */
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;
	attack();

	return 0;
}
