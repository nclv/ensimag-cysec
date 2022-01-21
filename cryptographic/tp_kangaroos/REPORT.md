# Memory generic discrete logarithm computation in an interval using kangaroos

By **Elina Houde** and **Nicolas Vincent**.

Program compilation (check that your are in the root directory and that obj/ and bin/ directories exist) :
```bash
make

# if you use clangd, use make clean; bear -- make to generate the compile_commands.json file and
# enable editor integration.
```

## Preparatory work

### Q1.
The simplest algorithm to compute discrete logarithm is **exhaustive search**. To find log_g(h) compute all successive powers of g until you find one equal to h. This algorithm works for any group but its running time is *proportional to the size of the group*.

This is not feasible here as G prime order is about 2^{109.6}.

### Q2.
We apply the **fast exponentiation algorithm** `num128 fast_exp(uint64_t g, uint64_t x)` using the provided function `mul11585` that compute the multiplication in the field F_2^{115}-85 (G is a subgroup of this field) and a generator of G.


### Q3 (Bonus).
Multiplication of two number in the field F_2^{115}-85.

Multiplication in a finite field is multiplication modulo an irreducible reducing polynomial used to define the finite field.

## Implementation kangaroos

### Q4.


### Q5.


### Q6.

See https://arxiv.org/pdf/1501.07019.pdf p.8-11



### Q7.


