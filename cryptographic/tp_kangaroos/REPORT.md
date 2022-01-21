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

We use an iterating function determining the next location of the kangaroos: pos_{i+1} = pos_i.g^f(pos_i), where f(pos_i) is a function that outputs the next jump size for the kangaroo and the kangaroo moves forward by this value.
We think of the **exponents as travel distances**, which should be small in comparison with the size of the group (search space).

The function f(pos_i) randomly selects values from a set S and only depends on the current location pos_i. In the original paper (Pollard), S is a set containing powers of 2 starting with 2^0 up to a limit with the largest value in the set being such that the mean of the values in the set is a certain (optimized) value µ.

We tested with f(pos_i) = 2^{hash(pos_i) % k + 1} with k the size of the set S but it was not performant as the hashing of pos_i is very long.

A simple way to implement the distinguished point set is to define a point to be distinguished if the n lowest bits in the representation of the point as a binary string are zero. Then the proportion of distinguished points is theta = 1/2^F. 
In general, F should be chosen small enough such that 1/theta = 2^F is small compared with sqrt(W), but not too small since the algorithm has to store an expected number of 2.theta.sqrt(W) distinguished points.

### Q5.


### Q6.

See https://arxiv.org/pdf/1501.07019.pdf p.8-11



### Q7.
When we decrease the value of p in our distinguisher, there are less distinguished points.

If we use a not so random function for our exponents, `exponent = (rand() % 2 == 0) ? mu - std : mu + std;` (biased to high values, only two different values, µ + std may overflow), we observe that the program take a very long time (nothing was found yet). This is a bad distribution of mean µ because there are only 2 very big values. 


## External sources

[Solving Elliptic Curve Discrete Logarithm Problem Using Parallelized Pollard’s Rho and Lambda Methods (paper)](https://ece.uwaterloo.ca/~p24gill/Projects/Cryptography/Pollard's_Rho_and_Lambda/Project.pdf)
[Solving Elliptic Curve Discrete Logarithm Problem Using Parallelized Pollard’s Rho and Lambda Methods (code)](https://ece.uwaterloo.ca/~p24gill/Projects/Cryptography/Pollard's_Rho_and_Lambda/Pollard's_Lambda_Method.html)
[How Long Does it Take to Catch a Wild Kangaroo?](https://arxiv.org/pdf/0812.0789.pdf)
[How can I generate random numbers with a normal or Gaussian distribution?](http://c-faq.com/lib/gaussian.html)