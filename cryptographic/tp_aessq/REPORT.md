# AES

By **Elina Houde** and **Nicolas Vincent**.

Program compilation (check that your are in the root directory) :
```
make
```

## Exercice 1: Warming up

### Q1.
`xtime` multiply in the field F_2[X]/X^8 + X^4 + X^3 + X + 1. In fact, it takes a polynomial P and compute P * X mod Q with Q = X^8 + X^4 + X^3 + X + 1. For this, the LFSR of Gaulois is used.

**Explications:**
In the function, we begin to look at the most significant bit to know if the multiplication will result in a number which are not in the field.
So, it the most significant bit is 0, we just need to shift to the left p (so, m=0). In the other case, we need to shift to the left p and to compute modulo Q, that means to xor Q to (p << 1) (so, m=0x1B = the reprensentation of Q).

```C
uint8_t xtime(uint8_t p) {
	uint8_t m = p >> 7;

	m ^= 1;
	m -= 1;
	m &= 0x1B; // 0x7B for X8 + X6 + X5 + X4 + X3 + X + 1

	return ((p << 1) ^ m);
}
```

**Theoric:**
P = p_7 X⁷ + p_6 X⁶ + p_5 X⁵ + p_4 X⁴ + p_3 X³ + p_2 X² + p_1 X + p_0
Q = X⁸ + X⁶ + X⁵ + X⁴ + X³ + X + 1
PX mod Q = p_6 X⁷ + p_5 X⁶ + p_4 X⁵ + (p_3 + p_7) X⁴ + (p_2 + p_7) X³ + p_1 X² + (p_0 + p_7) X + p_7
If p_7 =1:
    PX mod Q = p_6 X⁷ + p_5 X⁶ + p_4 X⁵ + p_3 X⁴ + p_2 X³ + p_1 X² + p_0 X = PX
Else:
    PX mod Q = p_6 X⁷ + p_5 X⁶ + p_4 X⁵ + p_3 X⁴ + p_2 X³ + p_1 X² + p_0 X + (X⁸ + X⁶ + X⁵ + X⁴ + X³ + X + 1) = PX + Q

**With the algorithm:**
If p_7 =1:
    ```C
    uint8_t m = p >> 7;  // m=1

	m ^= 1;  // m=0
	m -= 1;  // m=0xFF
	m &= 0x1B; // m = 0x7B (representation of Q)

	return ((p << 1) ^ m);  // return P*X + Q
    ```
Else:
    ```C
    uint8_t m = p >> 7;  // m=0

	m ^= 1;  // m=1
	m -= 1;  // m=0
	m &= 0x1B; // m = 0 (representation of Q)

	return ((p << 1) ^ m);  // return P*X
    ```

**Tests:**
With made two tests : one for a polynome with the the most significant bit egal to 0 and one one for a polynome with the the most significant bit egal to 1.

```C
void question1() {
	printf("-- Question 1 : Verification of the xtime function --\n");
	// Test with P = X + 1
	uint8_t mult = xtime(3);
	printf("Expected result : 0x6. Result : 0x%x.\n", mult);

	// Test with P = X^7 + X^4 + X^3 + X + 1
	mult = xtime(0x9B);
	printf("Expected result : 0x2D. Result : 0x%x.\n", mult);	
	printf("\n");
}
```

Result :
```bash
-- Question 1 : Verification of the xtime function --
Expected result : 0x6. Result : 0x6.
Expected result : 0x2D. Result : 0x2d.
```

**Variant of xtime**
So, to write the multiplication by x for a different representation of F_2⁸, we just need to replace the value 0x1B by the representation of Q over the new field $F_2[X]/Q, i.e. '7B' for Q = X^8 + X^6 + X^5 + X^4 + X^3 + X + 1 for instance.

```C
uint8_t xtime_variant(uint8_t p) {
	uint8_t m = p >> 7;

	m ^= 1;
	m -= 1;
	m &= 0x7B;

	return ((p << 1) ^ m);
}
```

### Q2.
See code (function `next_aes128_round_key` for the implementation and the function question2 for the tests with the test values provided in the standard document).

### Q3.
We need k1 != k2 because otherwise xored result is an array of zeros. In fact, if k_1 = k_2, E(k_1, x)=E(k_2, x), so F(k_1||k_2, x) = E(k_1, x) xor E(k_2, x) = E(k_1, x) xor E(k_1, x) = 0.

The distinguisher for the E has the following property:
Xor E(k, x) for x in lambda_set = vector 0

So, Sum F(k_1||k_2, x) for x in lambda_set = Xor (E(k_1, x) xor E(k_2, x)) for x in lambda_set
        = (Xor E(k_1, x) for x in lambda_set) xor (Xor E(k_1, x) for x in lambda_set)
        = vector 0 xor vector 0
        = vector 0

The 3-round square distinguisher for such an E also works for the corresponding F.


Test program :
We test that the 3-round square distinguisher for such an E also works for the corresponding F by computing the encryption of a lambda set and xoring the encrypted lambda set for each coordinate :

```C
uint8_t lambda_set[AES_LAMBDA_SET_SIZE][AES_BLOCK_SIZE];
uint8_t enc_lambda_set[AES_LAMBDA_SET_SIZE][AES_BLOCK_SIZE];
uint8_t sum_coordinates[AES_BLOCK_SIZE] = {0};
build_random_lambda_set(lambda_set);
for (size_t i = 0; i < AES_LAMBDA_SET_SIZE; i++) {
    f_construction(key1, key2, lambda_set[i], enc_lambda_set[i]);
    
    for(size_t coordinate=0; coordinate < AES_BLOCK_SIZE; coordinate++) {
        sum_coordinates[coordinate] ^= enc_lambda_set[i][coordinate];
    }
}
printf("Verification of the distinguisher's property (xor coordinates for a lambda set is the vector 0):\n");
print_array(sum_coordinates);
```

See the function `f_construction` for the implementation of the keyed function F and the function question3 for the test with k_1=k_2 and the verification of the distinguisher property on a random lambda set.


## Exercice 2: Key-recovery attack for 3¹/²-round AES

### Q1.

See the file `aes-128_attack.c`for the implementation of the attack and the explications of each function.

### Q2.

See the `main` function from the file `aes-128_enc.c` for the tests with `xtime_variant` and a random S-box.