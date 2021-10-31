#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "hash_table.h"
#include "second_preim_48_fillme.h"
#include "uthash.h" // https://troydhanson.github.io/uthash/
#include "utils.h"
#include "xoshiro256starstar.h"

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

// Returns the two 24 bits lower and upper into the lower 48 bits of an "upper's
// type" ie. if lower and upepr are 2 uint32_t you need to cast upper to
// uint64_t to keep the 48 bits and not only the first 32 bits of the result.
#define STORE_48(lower, upper) ((upper << 24) | lower)

/*
 * the 96-bit key is stored in four 24-bit chunks in the low bits of
 k[0]...k[3].
 * the 48-bit plaintext is stored in two 24-bit chunks in the low bits of p[0],
 * p[1].
 * the 48-bit ciphertext is written similarly in c
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

/**
 * @brief test the speck48_96 function
 *
 * @return int, 1 if equals, 0 otherwise
 */
int test_speck48_96(void) {
	// inversed order of the chunks of the key
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

	return assert_equals(c, expected, sizeof(c), sizeof(expected));
}

/**
 * @brief inverse the speck48_96 function
 *
 * @param k, input four 24-bit chunks of the key in the low bits of k[0]...k[3]
 * @param c, input two 24-bit chunks of the ciphertext in the low bits of
 * c[0],c[1]
 * @param p, output two 24-bit chunks of the plaintext in the low bits of
 * p[0],p[1]
 */
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

/**
 * @brief test the speck48_96_inv function
 *
 * @return int, 1 if equals, 0 otherwise
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

	return assert_equals(p, p_original, sizeof(p), sizeof(p_original));
}

/**
 * @brief The Davies-Meyer compression function based on speck48_96, using an
 * XOR feedforward. The input/output chaining value is given on the 48 low bits
 * of a single 64-bit word, whose 24 lower bits are set to the low half of the
 * "plaintext"/"ciphertext" (p[0]/c[0]) and whose 24 higher bits are set to the
 * high half (p[1]/c[1]).
 *
 * @param m
 * @param h, input on the 48 lower bits
 * @return uint64_t, compression function output in the 48 lower bits
 */
uint64_t cs48_dm(const uint32_t m[4], const uint64_t h) {
	// Get the 24 lower and upper bits
	const uint32_t p[2] = {h & 0xFFFFFF, (h >> 24) & 0xFFFFFF};

	uint32_t c[2] = {0};
	// Compute c = E(m, h) (the key is the message m)
	speck48_96(m, p, c);

	// Store the result in the lower 48 bits and xor with h
	return STORE_48(c[0], (uint64_t)c[1]) ^ h;
}

/**
 * @brief test the cs48_dm function
 *
 * @return int, 1 if equals, 0 otherwise
 */
int test_cs48_dm(void) {
	const uint64_t expected = 0x7FDD5A6EB248ULL;

	printf("Expected result of the compression function :\n");
	printf("%lx\n", expected);

	// call the compression function on the all-zero input
	const uint32_t m[4] = {0, 0, 0, 0};
	uint64_t c = cs48_dm(m, 0);

	printf("Result :\n");
	printf("%lx\n", c);

	return c == expected;
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
			printf("@%lu : %06X %06X %06X %06X => %06lX\n", i, mp[0], mp[1],
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
			printf("@%lu : %06X %06X %06X %06X => %06lX\n", fourlen, pad[0],
				   pad[1], pad[2], pad[3], h);
	}

	return h;
}

/**
 * @brief Computes the unique fixed-point of the function cs48_dm for the
 * message m
 *
 * @param m
 * @return uint64_t, fixed-point of the function cs48_dm for the message m on
 * the 48 lower bits
 */
uint64_t get_cs48_dm_fp(uint32_t m[4]) {
	// cs48_dm(m, fp) = fp means E(m, fp) ^ fp = fp.
	// So, we have E(m, fp) = 0, i.e. E^-1(m, 0) = fp

	const uint32_t c[2] = {0};
	uint32_t fp[2] = {0};
	// Compute fp = E^-1(m, c)
	speck48_96_inv(m, c, fp);

	// Store the result in the lower 48 bits
	return STORE_48(fp[0], (uint64_t)fp[1]);
}

/**
 * @brief test the cs48_dm_fp function
 *
 * @return int, 1 if equals, 0 otherwise
 */
int test_cs48_dm_fp(void) {
	uint32_t m[4] = {0};
	random_message(m);

	printf("Message: ");
	print_array(m, 4);

	// check that fp = cs48_dm(m, fp)
	uint64_t fp = get_cs48_dm_fp(m);
	uint64_t expected = cs48_dm(m, fp);

	return expected == fp;
}

/**
 * @brief Finds a two-block expandable message for hs48, using a fixed-point
 * That is, computes m1, m2 s.t. hs48_nopad(m1||m2) = hs48_nopad(m1||m2^*),
 * where hs48_nopad is hs48 with no padding.
 *
 * @param m1
 * @param m2
 */
void find_exp_mess(uint32_t m1[4], uint32_t m2[4]) {
	// Initialize the random generator with a custom seed
	uint64_t seed[4] = {2, 0, 2, 1};
	xoshiro256starstar_random_set(seed);

	printf("Building the m1 messages set\n");
	// We store the compression function outputs in a hash table
	hash_msg *hash_table = NULL; /* important! initialize to NULL */
	hash_msg *hi = NULL;

	// N = 2^(48/2) = 2^24
	size_t N = (1 << 24);
	uint32_t m[4] = {0};
	uint64_t h;
	// We fill the hash table with the couples (h, m)
	// h = cs48_dm(m, IV)
	for (size_t i = 0; i < N; ++i) {
		// Build the m1 message
		random_message(m);
		// Get the compression function output
		h = cs48_dm(m, IV);
		// Check that this entry was not yet inserted in the hash table
		HASH_FIND(hh, hash_table, &h, sizeof(uint64_t), hi);
		if (hi == NULL) {
			hi = new_hash_entry(h, m);
			HASH_ADD(hh, hash_table, h, sizeof(uint64_t), hi);
		}

		hi = NULL;
	}

	printf("Search the hash table for a collision\n");

	time_t endwait;
	time_t start = time(NULL);
	time_t seconds = 60 * 2; // end loop after 2 minutes

	endwait = start + seconds;

	printf("start time is : %s", ctime(&start));

	// We try to find a collision of h = get_cs48_dm_fp(m) with an entry of the
	// hash table.
	while (hi == NULL && (start < endwait)) {
		// Build the m2 message
		random_message(m);
		// Get the fixed point
		h = get_cs48_dm_fp(m);

		// Check that this entry was not yet inserted in the hash table
		HASH_FIND(hh, hash_table, &h, sizeof(uint64_t), hi);
		start = time(NULL);
	}

	printf("end time is : %s", ctime(&start));
	printf("Collision search ended\n");

	if (hi != NULL) {
		printf("Collision found!!!\n");
		// m contains m2, hi contains m1
		for (uint8_t i = 0; i < 4; ++i) {
			m1[i] = hi->m[i];
			m2[i] = m[i];
		}
	} else {
		printf("Collision not found...\n");
	}

	delete_all(hash_table);

	printf("Hash table was deleted\n");
}

/**
 * @brief test the find_exp_mess function
 *
 * @return int, 1 if equals, 0 otherwise
 */
int test_find_exp_mess(void) {
	// m = m1 || m2 || m2 || m2
	uint32_t m[16];
	// Fill m[0..8]
	find_exp_mess(m, &(m[4]));

	// Fill m[9..16]
	for (size_t i = 4; i < 8; i++) {
		m[i + 4] = m[i];
		m[i + 8] = m[i];
	}

	uint64_t h = hs48(
		m, 1, 0, 1); // message len = 1 * (4 * 24) = 96, padding zero, verbose 1
	uint64_t h2 = hs48(
		m, 2, 0, 1); // message len = 2 * (4 * 24), padding zero, verbose 1
	uint64_t h3 = hs48(
		m, 4, 0, 1); // message len = 4 * (4 * 24), padding zero, verbose 1

	return (h == h2) && (h == h3);
}


/**
 * @brief returns a 2^18 block message
 * 
 * @return uint32_t* 
 */
uint32_t *mess_18(void) {
	uint32_t *mess = malloc((1 << 20) * sizeof(*mess));
	if (mess == NULL) {
		fprintf(stderr, "couldn't allocate mess");
		return NULL;
	}

	// Generate the message
	for (size_t i = 0; i < (1 << 20); i += 4) {
		mess[i + 0] = (uint32_t)i;
		mess[i + 1] = 0;
		mess[i + 2] = 0;
		mess[i + 3] = 0;
	}

	return mess;
}

/**
 * @brief fixed-point expandable message attack of the hash function hs48
 * 
 */
void attack(void) {
	// Compute a message of 2^18 blocks
	uint32_t *mess = mess_18();
	printf("Hash of mess : %lx\n", hs48(mess, (1 << 18), 0, 0));

	// Compute the hi and store them in the hash table
	hash_msg *hash_table = NULL; /* important! initialize to NULL */
	hash_msg *hi = NULL;

	uint64_t h = IV;
	for (size_t i = 0; i < (1 << 18); i++) {
		// Get the compression function output
		h = cs48_dm(mess + 4 * i, h);
		// Check that this entry was not yet inserted in the hash table
		HASH_FIND(hh, hash_table, &h, sizeof(uint64_t), hi);
		if (hi == NULL) {
			hi = new_hash_entry(h, mess + 4 * i);
			hi->i = (uint32_t)i;
			HASH_ADD(hh, hash_table, h, sizeof(uint64_t), hi);
		}

		hi = NULL;
	}

	printf("Begin of the attack\n");

	// Find an expandable message
	uint32_t *mess2 = malloc((1 << 20) * sizeof(*mess2));
	if (mess2 == NULL) {
		fprintf(stderr, "couldn't allocate mess2");
		return;
	}

	find_exp_mess(mess2, mess2 + 4);
	uint64_t fp = get_cs48_dm_fp(mess2 + 4);

	// Search for a collision block
	uint32_t cm[4];
	uint64_t seed[4] = {2, 0, 2, 1};
	xoshiro256starstar_random_set(seed);

	printf("Search for a collision\n");
	time_t endwait;
	time_t start = time(NULL);
	time_t seconds = 60 * 10; // end loop after 10 minutes

	endwait = start + seconds;

	printf("start time is : %s", ctime(&start));

	// We try to find a collision of h = cs48_dm(cm, fp) with an entry of the
	// hash table.
	while (hi == NULL && (start < endwait)) {
		// Build the m2 message
		random_message(cm);
		// Get the hash
		h = cs48_dm(cm, fp);

		// Check that this entry was not yet inserted in the hash table
		HASH_FIND(hh, hash_table, &h, sizeof(uint64_t), hi);
		start = time(NULL);
	}

	printf("end time is : %s", ctime(&start));

	printf("Collision search ended\n");

	if (hi != NULL) {
		printf("Collision found!!!\n");

		printf("Build of the second preimage message\n");
		// For a collision on hi, we need to lengthen the expandable message to
		// i-1 blocks
		for (size_t i = 8; i < hi->i * 4; i += 4) {
			mess2[i] = mess2[4];
			mess2[i + 1] = mess2[5];
			mess2[i + 2] = mess2[6];
			mess2[i + 3] = mess2[7];
		}

		// Then, we copy the collision block in mess2
		mess2[hi->i * 4] = cm[0];
		mess2[hi->i * 4 + 1] = cm[1];
		mess2[hi->i * 4 + 2] = cm[2];
		mess2[hi->i * 4 + 3] = cm[3];

		// We suffixes the remaining blocks identical to the ones of mess
		for (size_t i = (hi->i + 1) * 4; i < (1 << 20); i++) {
			mess2[i] = mess[i];
		}
	} else {
		printf("Collision not found...\n");
	}

	// Should be equal :
	// printf("%ld\n", hs48(mess, (hi->i + 1), 0, 0));
	// printf("%ld\n", hs48(mess2, (hi->i + 1), 0, 0));

	delete_all(hash_table);

	printf("Hash table was deleted\n");

	printf("Test: %s\n\n",
		   hs48(mess, (1 << 18), 0, 0) == hs48(mess2, (1 << 18), 0, 0)
			   ? "true"
			   : "false");

	
	free(mess);
	mess = NULL;
	free(mess2);
	mess2 = NULL;
}

void part1(void) {
	printf("---Part one: preparatory work---\n");
	int err = test_speck48_96();
	printf("Test speck48_96 is correct: %s\n\n", err ? "true" : "false");

	err = test_speck48_96_inv();
	printf("Test speck48_96_inv is correct: %s\n\n", err ? "true" : "false");

	err = test_cs48_dm();
	printf("Test cs48_dm is correct: %s\n\n", err ? "true" : "false");

	err = test_cs48_dm_fp();
	printf("Test get_cs48_dm_fp is correct: %s\n\n", err ? "true" : "false");
}

void part2(void) {
	printf("\n---Part two: the attack---\n");
	int err = test_find_exp_mess();
	printf("Test find_exp_mess is correct: %s\n\n", err ? "true" : "false");

	attack();
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	part1();
	part2();

	return 0;
}
