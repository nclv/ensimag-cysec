#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "mul11585.h"
#include "kangaroos.h"


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
 * w_{i+1} = w_i + f(g^w_i) = x + d_{i+1} := w_i.g^r(e_i). Note that w_i = h.g^c_i 
 * d_{i+1} = d_i + f(h.g^d_i) := ?
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
    
    while(x > 0) {
        if (x % 2 == 1) {
            res = mul11585(res, acc);
        }
        acc = mul11585(acc, acc);
        x >>= 1;
    }

    return res;
}


/**
 * @brief compute the exponentiation map [[0, 2^64-1]] -> G, x -> g^x with g = 4398046511104 a generator of G
 *
 * @param x
 * @return num128, the result of the exponentiation of g^x
 */
num128 gexp(uint64_t x) {
    return fast_exp(4398046511104, x);
}

/**
 * @brief test the gexp function
 *
 * @return int, 1 if equals, 0 otherwise
 */
int test_gexp(void) {                                
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
    return (result.t[0] == expected.t[0]) & (result.t[1] == expected.t[1]);
}

int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

    printf("---Preparatory work---\n");
    bool err = test_gexp();
	printf("Test gexp is correct: %s\n\n", err ? "true" : "false");

    printf("---Implementing kangaroos---\n");

	return 0;
}