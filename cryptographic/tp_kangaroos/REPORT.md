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

The superior parts of the product (a1b1 and then mid_q) are multiplicated by $(2^{128}-2^{115})*85 = 696320$ which corresponds to the computation $x * 2^{128} \mod (2^{115}-85)$ (the superior part of the product is the part above the $2^{128}$).

Then, each intermediate result is reduce with the formula ((x >> 115) * 85 + (x & m115.s)). If we define $x = x1 * 2^{115} + x2$,
((x >> 115) * 85) + (x & m115.s) corresponds to the computation $x1 \mod (2^{115}-85) + x2$. This result of this computation is $x \mod (2^{115}-85)$ in the best case, $y = (x \mod (2^{115}-85)) + (2^{115}-85))$ otherwise. 


Finaly, as the result $res$ can be superior to $2^{115}-85$ but is inferior to $(2^{115}-85) * 2$, we remove, if necessary, $2^{115}-85$, to have $res = res \mod (2^{115}-85)$.



## Implementation kangaroos

We use an iterating function determining the next location of the kangaroos: $pos_{i+1} = pos_i.g^f(pos_i)$, where $f(pos_i)$ is a function that outputs the next jump size for the kangaroo and the kangaroo moves forward by this value.

The function $f(pos_i)$ randomly selects values from a set $S$ and only depends on the current location pos_i. In the original paper (Pollard), S is a set containing powers of $2$ starting with $2^0$ up to a limit with the largest value in the set being such that the mean of the values in the set is a certain (optimized) value $µ$.

### Q4.
We chose to comply with the heuristic analysis of Steven D. Galbraith for the parameterisation of *k, µ, d and W*.
So, we take :
- $W = 2^{64}$, the size of the interval where we need to search for logarithms;
- $k = log(W)/2 = 32$;
- $µ = sqrt{W}/2 = 2^{31}$;
- $d = log(W)/sqrt(W) = 1/2^{27}$.

Similarly, we follow the suggestion of Steven D. Galbraith to instantiate the exponents $e_{0,...,k}$ :
- the first $k-1$ exponents $e_i$ take the values $2^{i}$ (for $i$ in $[1, k-1]$);
- the last exponent $e_{k}$ is instantiated to $µ - \sum_{i=1}^{k-1} e_i$.

We think of the **exponents as travel distances**, which should be small in comparison with the size of the group (search space).

For the set $S_{1,...,k}$, we suppose that the values in the subgroup are uniformely distributed in $[0, 2^{115}-85]$ and chose to divise the subgroup via the modulo operator : the number $x$ belongs to the set $S_j$ with $j = x \mod k$.

A simple way to implement the distinguished point set is to define a point to be distinguished if the $n$ lowest bits in the representation of the point as a binary string are zero. Then the proportion of distinguished points is $\theta = 1/2^n$. 
In general, $n$ should be chosen small enough such that $1/\theta = 2^n$ is small compared with $sqrt(W)$, but not too small since the algorithm has to store an expected number of $2.theta.sqrt(W)$ distinguished points.

We define the distinguished elements as the element which are devisible by $q = 1/p = 1/d$ and the distinguisher function $D$ as : 
- if $x \mod k = 0$, $D$ : $x \rightarrow 1$;
- if $x \mod k \neq 0$, $D$ : $x \rightarrow 0$.

### Q5

We have to be carefull with the types maximum value. An `uint64_t` only goes to $2^{63} - 1$. There might be issues if we call directly function like `log` on $W$.

### Q6.
If the expected jump size (exponent value) from a set of jump (exponents) values is $µ$ and the distance that needs to be covered is $d$ then, the number of jumps is given by the following equation: **mean_jump_size x number_of_jumps = distance_covered**.
Hence, the number of jumps (exponents, i.e. $k$) needed to cover distance $d$ is $d / µ$.

Choosing the exponents in the set of jumps as powers of two works fine in principal in the sense that the theoretically predicted running times are almost met.

The algorithm require only $O(log(W))$ memory thanks to the set of distinguished elements.

### Q7.
When we decrease the value of $p$ in our distinguisher, there are less distinguished points. After a threshold, there are not enough distinguished points to compute the discrete log.

If we use a not so random function for our exponents, `exponent = (rand() % 2 == 0) ? mu - std : mu + std;` (biased to high values, only two different values, $µ + std$ may overflow), we observe that the program take a very long time (nothing was found yet). This is a bad distribution of mean $µ$ because there are only 2 very big values. Recall exponents should be small in comparison with the size of the group.

Then we used another random function : `exponent = (uint64_t) rand() % (2 * mu);`. We observe that the mean value of the exponents is close to $µ$. Choosing the exponents uniformly at random from the interval $[0, 2µ]$ yielded good results (2min40 with $k = 36$ and 2min10s with $k = 32$).


We modified the value of $k$ from $log(W) / 2 = 32$ to a value that gave us a mean closer to $µ$ with the power of two: $log(sqrt(W)) + log(log(sqrt(W))) - 1 = 32 + 5 - 1 = 36$. It had no major impact on the runtime (1min24s with $k = 32$ vs. 2min40s with $k = 36$).

## External sources

[Solving Elliptic Curve Discrete Logarithm Problem Using Parallelized Pollard’s Rho and Lambda Methods (paper)](https://ece.uwaterloo.ca/~p24gill/Projects/Cryptography/Pollard's_Rho_and_Lambda/Project.pdf)
[Solving Elliptic Curve Discrete Logarithm Problem Using Parallelized Pollard’s Rho and Lambda Methods (code)](https://ece.uwaterloo.ca/~p24gill/Projects/Cryptography/Pollard's_Rho_and_Lambda/Pollard's_Lambda_Method.html)
[How Long Does it Take to Catch a Wild Kangaroo?](https://arxiv.org/pdf/0812.0789.pdf)
[How can I generate random numbers with a normal or Gaussian distribution?](http://c-faq.com/lib/gaussian.html)
[Kangaroo Methods for Solving the Interval Discrete Logarithm Problem](https://arxiv.org/pdf/1501.07019.pdf)