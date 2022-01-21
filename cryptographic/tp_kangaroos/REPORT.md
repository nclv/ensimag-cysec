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

This is not feasible here as G prime order is about $2^{109.6}$.

### Q2.
We apply the **fast exponentiation algorithm** `num128 fast_exp(uint64_t g, uint64_t x)` using the provided function `mul11585` that compute the multiplication in the field $F_2^{115}-85$ (G is a subgroup of this field) and a generator of G.


### Q3 (Bonus).
Multiplication of two number in the field F_2^{115}-85.

Multiplication in a finite field is multiplication modulo an irreducible reducing polynomial used to define the finite field.

## Implementation kangaroos

### Q4.
We chose to comply with the heuristic analysis of Steven D. Galbraith for the parameterisation of *k, µ, d and W*.
So, we take :
- $W = 2^{64}$, the size of the interval where we need to search for logarithms;
- $k = log(W)/2 = 32$;
- $µ = sqrt{W}/2$ = 2^{31}$;
- $d = log(W)/sqrt(W) = 1/2^{27}$.

Similarly, we follow the suggestion of Steven D. Galbraith to instantiate the exponents $e_{0,...,k}$ :
- the first $k-1$ exponents $e_i$ take the values $2^{i}$ (for $i$ in $[1, k-1]$);
- the last exponent $e_{k}$ is instantiated to $µ - \sum_{i=1}^{k-1} e_i$.

For the set $S_{1,...,k}$, we suppose that the values in the subgroup are uniformely distributed in $[0, 2^{115}-85]$ and chose to divise the subgroup via the modulo operator : the number $x$ belongs to the set $S_j$ with $j = x \mod k$.

We define the distinguished elements as the element which are devisible by $q = 1/p = 1/d$ and the distinguisher function $D$ as : 
- if $x \mod k = 0$, $D$ : $x \rightarrow 1$;
- if $x \mod k \neq 0$, $D$ : $x \rightarrow 0$.


### Q5.1


### Q6.



### Q7.


