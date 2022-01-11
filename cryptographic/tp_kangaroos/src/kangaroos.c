#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "mul11585.h"
#include "kangaroos.h"


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