#include <bits/stdint-uintn.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>

#include "hash_table.h"
#include "kangaroos.h"
#include "lambda.h"
#include "mul11585.h"
#include "xoshiro256starstar.h"

/* According to the heuristic analysis */

// W : size of the interval
// W = 2^64

// k : number of subsets of G, number of exponents e_j
#define k 36 // log2(W) / 2

#define mu (1ULL << 31) // sqrt(W) / 2 = 2^31

// d = p : proba to obtain a distinguished number
// p = log2(W) / sqrt(W) = 32/2^32 = 2^5 / 2^32 = 2^(5-32) = 1/2^27
#define q (1ULL << 26) // q = 1/p

/**
 * https://ece.uwaterloo.ca/~p24gill/Projects/Cryptography/Pollard's_Rho_and_Lambda/Pollard's_Lambda_Method.html
 * https://ece.uwaterloo.ca/~p24gill/Projects/Cryptography/Pollard's_Rho_and_Lambda/Project.pdf
 * https://arxiv.org/pdf/0812.0789.pdf
 *
 * Pollard's Kangaroo (Lambda) method
 *
 * Given g, h ∈ G, solve for x ∈ [a, b] with h = g^x
 * Here a = 0, b = W = 2^64 - 1 << N order of G
 *
 * Preliminaries.
 * Define a set D \in G of distinguished points.
 * Define a set S of jump sizes {s_0, ..., s_d}. S = {2^k / 0 <= k <= d}
 * Choose d such that jump sizes average to µ
 *
 *
 * t_0 = W / 2, w_0 = x := h (i.e. g^x), d_0 = 0. Note that g^w_0 = h.g^d_0.
 *
 * t_{i+1} = t_i + f(g^t_i) := t_i.g^r(e_i). Note that t_i = h.g^b_i
 * w_{i+1} = w_i + f(g^w_i) = x + d_{i+1} := w_i.g^r(e_i). Note that w_i =
 * h.g^c_i d_{i+1} = d_i + f(h.g^d_i) := ?
 *
 * if g^t_i := t_i \in D, store (g^t_i, t_i - t_0) := (t_i, b_i)
 * if g^w_i = h.g^d_i := w_i \in D, store (g^w_i, d_i) := (w_i, c_i)
 *
 * hash table : (t_i, total_jumped_distance)
 * k = random(0, W)
 */

/**
 * @brief compute the exponentiation g^x in the field F^{x}_{2^115-85}
 *
 * @param g
 * @param x
 * @return num128, the result of the exponentiation of g^x
 */
num128 fast_exp(uint64_t g, uint64_t x) {
	num128 res = {.t = {1, 0}};
	num128 acc = {.t = {g, 0}};

	while (x > 0) {
		if (x % 2 == 1) {
			res = mul11585(res, acc);
		}
		acc = mul11585(acc, acc);
		x >>= 1;
	}

	return res;
}

/**
 * @brief compute the exponentiation map [[0, 2^64-1]] -> G, x -> g^x with g =
 * 4398046511104 a generator of G
 *
 * @param x
 * @return num128, the result of the exponentiation of g^x
 */
num128 gexp(uint64_t x) { return fast_exp(4398046511104, x); }

/**
 * @brief test the gexp function
 *
 * @return bool, true if equals, false otherwise
 */
bool test_gexp(void) {
	num128 expected;
	num128 result;
	uint64_t x;
	bool test = false;

	x = 257;
	expected.t[1] = 0x42F953471EDC0ULL;
	expected.t[0] = 0x0840EE23EECF13E4ULL;
	result = gexp(x);

	test = is_equal(result, expected) ? true : test;

	x = 112123123412345;
	expected.t[1] = 0x21F33CAEB45F4ULL;
	expected.t[0] = 0xD8BC716B91D838CCULL;
	result = gexp(x);

	test = is_equal(result, expected) ? true : test;

	x = 18014398509482143;
	expected.t[1] = 0x7A2A1DEC09D03ULL;
	expected.t[0] = 0x25357DAACBF4868FULL;
	result = gexp(x);

	test = is_equal(result, expected) ? true : test;

	return test;
}

/**
 * @brief compare two num128 variables
 *
 * @param result
 * @param expected
 * @return bool, true if equal, false otherwise
 */
bool is_equal(num128 result, num128 expected) {
	// or (result.t[0] == expected.t[0]) & (result.t[1] == expected.t[1]);
	return result.s == expected.s;
}

/*void fill_exponents(uint64_t *ej, uint64_t k, uint64_t mu) {
	// Initialize the random generator with a custom seed
	uint64_t seed[4] = {2, 0, 2, 1};
	xoshiro256starstar_random_set(seed);

	uint64_t residue = k * mu;
	for (uint64_t j = 0; j < k-1; j++) {
		ej[j] = xoshiro256starstar_random()/(uint64_t) pow(2, 64) * (residue >>
2); residue -= ej[j];
	}
	ej[k-1] = residue;
}*/

/**
 * @brief Pick k exponents e_j s.t. their average 1/k * Sum(e_j) equal µ.
 * Fill the given arrays
 *
 * @param ej, array for the k exponents 2^j, j in [0, k-1]
 * @param gexpej, array for the result of g^e_j for each exponent e_j
 * @return int, 1 if equals, 0 otherwise
 */
void fill_exponents(uint64_t *ej, num128 *gexpej) {
	uint64_t sum = 0;
	for (uint64_t j = 0; j < k - 1; j++) {
		ej[j] = ((uint64_t)(1) << j);
		gexpej[j] = gexp(ej[j]);
		sum += ej[j];
	}
	ej[k - 1] = mu * k - sum;
	gexpej[k - 1] = gexp(ej[k - 1]);
}

/**
 * @brief determine the subset S_j in which the variable x is, and provide the
 * exponent ej and g^ej associated to this subset
 *
 * @param ej, array of the exponents ej associated to the subsets S_j
 * @param gexpej, array of g^ej
 * @param x
 * @param gej, ptr to the variable that will be set to g^ej
 * @return the exponent ej
 */
uint64_t get_exponent_for_subset(uint64_t *ej, num128 *gexpej, num128 x,
								 num128 *gej) {
	uint64_t j = (x.t[0] % k);
	*gej = gexpej[j];
	return ej[j];
}

/**
 * @brief Define the distinguisher D : G → {0, 1} so that Pr[D(x) = 1 : x <<- G]
 * = p = 1/q
 *
 * @param x
 * @return bool, true if x is a distinguished element, false otherwise
 */
bool is_distinguished(num128 x) { return (x.t[0] % q) == 0; }

/**
 * @brief Add a trap (x, exponent) in the traps_list of a kangaroo
 *
 * @param traps_list, ptr to the traps list of the kangaroo
 * @param x, where to lay the trap
 * @param exponent, the exponent that allows to the kangaroo to land on x
 */
void add_trap(trap **traps_list, num128 x, uint64_t exponent) {
	trap *xi = NULL;
	HASH_FIND(hh, *traps_list, &x, sizeof(num128), xi);
	if (xi == NULL) {
		xi = new_trap(x, exponent);
		HASH_ADD(hh, *traps_list, x, sizeof(num128), xi);
	}
}

/**
 * @brief Jump of a kangaroo. If the kangaroo lands on a distinguished element
 * x, it lays a trap. However, if a trap is already present, it instead gets
 * trapped and provide this trap in element_trap. Update the param of the jump
 * of the kangaroo (position x, last_exponent).
 *
 * @param ej, array of the exponents ej associated to the subsets S_j
 * @param gexpej, array of g^ej
 * @param x, where the kangaroo land before the jump. Will be replace by the new
 * position of the kangaroo
 * @param last_exponent, sum of all the exponent used to jump
 * @param traps_list, ptr to the traps list of the kangaroo
 * @param verify_traps_list, traps list of the trap layed by another kangaroo
 * @param element_trap, containt the trap if the kangaroo gets trapped
 */
void jump(uint64_t *ej, num128 *gexpej, num128 *x, uint64_t *last_exponent,
		  trap **traps_list, trap *verify_traps_list, trap **element_trap) {

	num128 gej;
	uint64_t exponent = get_exponent_for_subset(ej, gexpej, *x, &gej);

	*x = mul11585(*x, gej);
	(*last_exponent) += exponent;

	if (is_distinguished(*x)) {
		add_trap(traps_list, *x, *last_exponent);
		HASH_FIND(hh, verify_traps_list, x, sizeof(num128), *element_trap);
	}
}

/**
 * @brief Solves the stated discrete logarithm problem using the kangaroo method
 *
 * @param target, discrete log to solve
 * @return result num128, result.t[1] = 1 if it failed to resolve the problem, 0
 * if it success with t[0]=the solution
 */
num128 dlog64(num128 target) {
	num128 result = {.t = {0, 0}};

	// Pick k exponents e_j s.T. their average 1/k * Sum(e_j) from j=1 to k ~= µ
	uint64_t ej[k];
	num128 gexpej[k];
	fill_exponents(ej, gexpej);

	trap *xi_traps = NULL; /* important! initialize to NULL */
	trap *yi_traps = NULL; /* important! initialize to NULL */
	trap *element_trap = NULL;
	trap *element_trap_y = NULL;

	// Initialisation of the tame kangaroo's sequences
	uint64_t b_exponent = 1ULL << 63; // W / 2 = 2^63
	num128 x = gexp(b_exponent);
	if (is_distinguished(x))
		add_trap(&xi_traps, x, b_exponent);

	// Initialisation of the wild kangaroo's sequences
	num128 y = target;
	uint64_t c_exponent = (uint64_t)0;
	if (is_distinguished(y))
		add_trap(&yi_traps, y, c_exponent);

	time_t endwait;
	time_t start = time(NULL);
	time_t seconds = 60 * 10; // end loop after 10 minutes
	endwait = start + seconds;

	printf("start time is : %s", ctime(&start));
	uint64_t round = 0;

	// Jumps of the kangaroo until the time was over or one the kangaroo is
	// trapped
	while (element_trap == NULL && element_trap_y == NULL &&
		   (start < endwait)) {
		// Compute a new xi (for the tame kangaroo)
		jump(ej, gexpej, &x, &b_exponent, &xi_traps, yi_traps, &element_trap);
		// Compute a new yi (for the wild kangaroo)
		jump(ej, gexpej, &y, &c_exponent, &yi_traps, xi_traps, &element_trap_y);

		start = time(NULL);
		round++;
	}

	printf("end time is : %s", ctime(&start));
	printf("\nNb de tour de boucle :%lx\n", round);

	// Get the result of the discrete logarithm problem if found
	if (element_trap != NULL || element_trap_y != NULL) {
		printf("TRAPPED !!!\n");
		if (element_trap_y != NULL) {
			if (c_exponent > element_trap_y->exponent) {
				result.t[0] = c_exponent - element_trap_y->exponent;
			} else {
				result.t[0] = element_trap_y->exponent - c_exponent;
			}
		} else {
			if (b_exponent > element_trap->exponent) {
				result.t[0] = b_exponent - element_trap->exponent;
			} else {
				result.t[0] = element_trap->exponent - b_exponent;
			}
		}
	} else {
		printf("ECHEC :(");
		result.t[1] = 1;
	}

	// Free the structures
	delete_all(xi_traps);
	delete_all(yi_traps);

	return result;
}

/**
 * @brief test that the average of the exponent is mu
 *
 * @return bool, true if equals, false otherwise
 */
bool test_fill_exponents(void) {
	// Pick k exponents e_j
	uint64_t ej[k];
	num128 gexpej[k];
	fill_exponents(ej, gexpej);

	uint64_t sum = 0;
	for (uint64_t j = 0; j < k; j++) {
		sum += ej[j];
		if (ej[j] == 0) {
			printf("Coef %lu nul\n", j);
			exit(0);
		}
	}

	return sum / k == mu;
}

void verify_trap(void) {
	// When the target is x_1, we got trapped

	num128 gej;
	uint64_t ej[k];
	num128 gexpej[k];
	fill_exponents(ej, gexpej);
	uint64_t b_exponent = 1ULL << 63; // W / 2 = 2^63
	num128 x = gexp(b_exponent);
	get_exponent_for_subset(ej, gexpej, x, &gej);

	num128 new_x = mul11585(x, gej);
	print_num128(new_x);
	dlog64(new_x);
}

/**
 * @brief test the dlog64 function
 *
 * @return bool, true if equals, false otherwise
 */
bool test_dlog64(void) {
	num128 target = {.t = {0xB6263BF2908A7B09ULL, 0x71AC72AF7B138ULL}};
	num128 result = dlog64(target);
	if (result.t[1] != 0) {
		printf("NOT FOUND");
		return false;
	}
	return is_equal(target, gexp(result.t[0]));
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

	printf("---Preparatory work---\n");
	printf("Test gexp is correct: %s\n\n", test_gexp() ? "true" : "false");

	printf("---Implementing kangaroos---\n");
	num128 target = {.t = {0xB6263BF2908A7B09ULL, 0x71AC72AF7B138ULL}};
	// dlog64(target);

	// max of uint64_t (8 bytes unsigned) is 2^64 - 1
	// see https://stackoverflow.com/a/67559486 for issues with log2

	// k
	// recommended: log2(2^64) / 2 = 32,
	// recommended closer to mu: log2(sqrt(2^64)) + log2(log2(sqrt(2^64))) - 1 =
	// 32 + 5 - 1 = 36
	// uint8_t k = 36;
	// mu
	// sqrt(2^64) / 2
	// uint64_t mu = 1UL << 31;

	uint64_t jump_size = next_jump_size(&target, k);
	printf("%lu\n", jump_size);

	// generate k powers of two
	uint64_t powers_of_two[k];
	generate_powers_of_two(powers_of_two, k);

	uint64_t sum = 0;
	for (size_t i = 0; i < k; ++i) {
		sum += powers_of_two[i];
	}
	printf("mu : %lu\n", mu);
	printf("Mean of the exponents : %lu\n", sum / k);
	printf("Difference |mu - mean| : %llu\n",
		   mu > sum / k ? mu - sum / k : sum / k - mu);
	// Choosing the exponents in the set of jumps as powers of two works fine in
	// principal in the sense that the theoretically predicted running times are
	// almost met. Choosing the exponents uniformly at random from the interval
	// [0, 2μ] (μ denoting the theoretically optimal mean value of the si)
	// yielded even slightly better results.

	// Initialize the random generator with a custom seed
	// uint64_t seed[4] = {2, 0, 2, 1};
	// xoshiro256starstar_random_set(seed);

	// lambda_method();
	lambda_method_simultaneous();

	return 0;
}