#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include "kangaroos.h"
#include "mul11585.h"
#include "xoshiro256starstar.h"
#include "hash_table.h"


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

uint64_t get_exponent_for_subset(uint64_t *ej, uint64_t k, num128 x) {
    // TODO : subsets S_j => x % k
    uint64_t i = (x.t[0] % k);
    return ej[i];
}

/*void fill_exponents(uint64_t *ej, uint64_t k, uint64_t mu) {
    // Initialize the random generator with a custom seed
	uint64_t seed[4] = {2, 0, 2, 1};
	xoshiro256starstar_random_set(seed);

    uint64_t residue = k * mu;
    for (uint64_t j = 0; j < k-1; j++) {
        ej[j] = xoshiro256starstar_random()/(uint64_t) pow(2, 64) * (residue >> 2);
        residue -= ej[j];
    } 
    ej[k-1] = residue;
}*/

void fill_exponents(uint64_t *ej, uint64_t k, uint64_t mu) {
    uint64_t sum = 0;
    for (uint64_t j = 0; j < k; j++) {
        ej[j] = (uint64_t) (1) << j;
        sum += ej[j];
        printf("%ld-", ej[j]);
    }
    printf("sum %ld, %ld\n", sum, ((1 << k) - 1) / k);
    printf("mu %ld\n", mu);
}

bool is_distinguished(num128 x, double p) {
    //printf("%lf\n", p);
    //printf("%ld\n", (uint64_t) (p * pow(2, 64)));
    if (x.t[0] % ((uint64_t) (1/p)) == 0 ) {
        printf("distinct");
    }
    return x.t[0] < p * pow(2, 64);
}

void add_trap(trap *traps_list, num128 x, uint64_t exponent) {
    trap *xi = NULL;
    HASH_FIND(hh, traps_list, &x, sizeof(num128), xi);
    if (xi == NULL) {
        xi = new_trap(x, exponent);
        HASH_ADD(hh, traps_list, x, sizeof(num128), xi);
    }
}

void jump(uint64_t *ej, uint64_t k, double p, num128 *x, uint64_t *last_exponent, trap *traps_list,
trap *verify_traps_list, trap *element_trap
) {
    uint64_t exponent = get_exponent_for_subset(ej, k, *x);
    *x = mul11585(*x, gexp(exponent));
    (*last_exponent) += exponent;
    if (is_distinguished(*x, p)) {
        add_trap(traps_list, *x, *last_exponent);
        HASH_FIND(hh, verify_traps_list, &x, sizeof(num128), element_trap);
    }
}


// Q4 : specify suitable values for k, µ, d, W 
// k : number of subsets of G, number of exponents e_j
// d = p : proba to obtain a distinguished number
// W : size of the interval
// and how to pick the exponents e_i, the sets S_i and D
num128 dlog64(num128 target) {
    //num128 result;
    double W = pow(2, 64);
    // According to the heuristic analysis
    uint64_t k = (uint64_t) log2(W) / 2 - 1;  // = 31
    uint64_t mu = (uint64_t) sqrt(W) / 2;
    double p = log2(W) / sqrt(W);

    // Pick k exponents e_j s.T. their average 1/k * Sum(e_j) from j=1 to k ~= µ
    uint64_t ej[k];
    fill_exponents(ej, k, mu);
    
    trap *xi_traps = NULL; /* important! initialize to NULL */
    trap *yi_traps = NULL; /* important! initialize to NULL */
    trap *element_trap = NULL;

    // Initialisation of the tame kangaroo's sequences
    num128 x = gexp((uint64_t) W/2);
    printf("x = g^W/2");
    print_num128(x);
    uint64_t b_exponent = (uint64_t) W/2;
    if (is_distinguished(x, p))
        add_trap(xi_traps, x, b_exponent);

    // Initialisation of the wild kangaroo's sequences
    num128 y = target;
    uint64_t c_exponent = (uint64_t) 0;
    if (is_distinguished(y, p))
        add_trap(yi_traps, y, c_exponent);


    time_t endwait;
	time_t start = time(NULL);
	time_t seconds = 60 * 10; // end loop after 10 minutes

	endwait = start + seconds;

	printf("start time is : %s", ctime(&start));

    while((start < endwait) && element_trap == NULL) {
        jump(ej, k, p, &x, &b_exponent, xi_traps, yi_traps, element_trap); // Compute a new xi
        if (element_trap != NULL) {
            printf("exp %ld", b_exponent - element_trap->exponent);
        } else {
            jump(ej, k, p, &y, &c_exponent, yi_traps, xi_traps, element_trap); // Compute a new yi
            if (element_trap != NULL)
                printf("exp %ld", c_exponent - element_trap->exponent);
        }    
    }

    printf("end time is : %s", ctime(&start));
    if (element_trap != NULL) {
        printf("TRAPPED !!!\n");
    } else {
        printf("ECHEC :(");
    }

    delete_all(xi_traps);
    delete_all(yi_traps);
    return target;
}



int main(int argc, char **argv) {
	(void)argc;
	(void)argv;

    printf("---Preparatory work---\n");
    bool err = test_gexp();
	printf("Test gexp is correct: %s\n\n", err ? "true" : "false");

    printf("---Implementing kangaroos---\n");
    num128 target = {.t = {0xB6263BF2908A7B09ULL, 0x71AC72AF7B138ULL}};
    dlog64(target);
	return 0;
}