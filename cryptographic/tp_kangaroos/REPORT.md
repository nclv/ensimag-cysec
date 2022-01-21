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
The simplest algorithm to compute discrete logarithm is **exhaustive search**. To find log_g(h), compute all successive powers of g until you find one equal to h. This algorithm works for any group but its running time is *proportional to the size of the group*.

This is not feasible here as G prime order is about $2^{109.6}$.

### Q2.
We apply the **fast exponentiation algorithm** `num128 fast_exp(uint64_t g, uint64_t x)` using the provided function `mul11585` that compute the multiplication in the field $F_2^{115}-85$ (G is a subgroup of this field) and a generator of G.


### Q3 (Bonus).

Multiplication in a finite field is multiplication modulo an irreducible reducing polynomial used to define the finite field.

The function `mul11585` compute the multiplication of two numbers a and b in the field $F_2^{115}-85$. For this, the number a is defined as a1a0 et the number b as b1b0.

The function begins by compute the product a1 * b1, a0 * b1, a1 * b0, b1 * b0 as the multiplication of a by b corresponds to $a1 * b1 * 2^{128} + a0 * b1 * 2^{64} + a1 * b0 * 2^{64} + b1 * b0$.

The superior parts of the product (a1b1 and then mid_q) are multiplicate by $(2^{128}-2^{115})*85 = 696320$ which corresponds to  

Then, each intermediate result is reduce with the formula ((x >> 115) * 85 + (x & m115.s)). If we define $x = x1 * 2^{115} + x2$,
((x >> 115) * 85) + (x & m115.s) corresponds to the computation $x1 \mod (2^{115}-85) + x2$. This result of this computation is $x \mod (2^{115}-85)$ in the best case, $y = (x \mod (2^{115}-85)) + (2^{115}-85))$ otherwise. 


Finaly, as the result $res$ can be superior to $2^{115}-85$ but is inferior to $(2^{115}-85) * 2$, we remove, if necessary, $2^{115}-85$, to have $res = res \mod (2^{115}-85)$.



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


### Q5.


### Q6.



### Q7.


