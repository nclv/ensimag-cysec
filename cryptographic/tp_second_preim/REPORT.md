# Second pre-image attack

## Introductory notes

**Objective :** given a l-block message M such that `H(M) = H(m_1||...||h_i)` with `i <= l`, if there is a i-block message `M'` such that `H(M') = h_i`, we may form the message `M''` such that `H(M'') = H(M'||m_{i+1}||...||m_l) = h_l`.  
Now, the objective is to find `M'` such that there exist `i` verifying the previous equality.  

Instead of having a single target we now have `l` targets : `2^n/l`.

If the (final) padding, who depends on the length of the message, doesn't match, we don't have an equality anymore. So we use expandable messages.

## Compilation

Use the `Makefile`. We use `gcc` but you can easily modify the `Makefile` to use `clang` (a few seconds faster on the attack) by modifying `CC` and `LD`.

## Part one : Preparatory work

- [Speck block ciphers (p.14)](https://eprint.iacr.org/2013/404.pdf)

### Q1.

We look at the speck parameter table. In our case the **block size** is 48, the **key size** is 96, the word size `n` is 24, there are 4 key words, *rot alpha* is equal to 8, *rot beta* is equal to 3 and there are 23 rounds.

We have to implement the following encryption loop :
```bash
x←c[0]
y←c[1]
for i = 0..22
    x←(S^{−α}.x + y)⊕k[i]  # ((ROTL24_16(c[0]) + c[1]) ^ rk[i]) & 0xFFFFFF;
    y←S^β.y⊕x  # ROTL24_3(c[1]) ^ c[0];
end for
```

`S^{−α}` is `ROTL24_16` ie. a circular shift of 8 bits to the right or 16 bits to the left (modulo 24 bits).  
`S^β` is `ROTL24_3` ie. a circular shift of 3 bits to the left.  
Addition is done modulo 2^24 with `& 0xFFFFFF`.

### Q2.

The function `spec48_96_inv` has the same key schedule step as in the function `spec48_96`. We must then implement the decryption loop ie. the same operations of the encryption loop but in the reverse order.

```bash
x←p[0]
y←p[1]
for i = 22..0
    y←S^{-β}.(y⊕x)  # ROTL24_21(p[1] ^ p[0]);
    x←S^{α}.(x⊕k[i]-y)  # ROTL24_8(((p[0] ^ rk[i]) - p[1]) & 0xFFFFFF);
end for
```

We use the macros `ROTL24_21` and `ROTL24_8` that respectively compute `S^{-β}` and `S^{α}`.

### Q3.

The implementation is already quite detailed. The difficulty lies in the masking and shifting before and after calling the function `speck48_96`. `h & 0xFFFFFF` extracts the 24 lower bits and `(h >> 24) & 0xFFFFFF` extracts the following 24 lower bits of `h`.

Finally the macro `STORE_48` do the reverse operation of merging two 24 bits lower and upper into the lower 48 bits.

### Q4.

We need to solve the equation `cs48_dm(m, fp) = fp` ie. `speck48_96(m, fp) ^ fp = fp`. This leads to `speck48_96(m, fp) = 0` ie. `speck48_96_inv(m, 0) = fp`.

Then we use the macro `STORE_48` to store `fp` in an `uint64_t`.

## Part two : the attack
### Q1.

While the birthday attack attempts to find two messages which *result in the same hash value*, the meet-in-the-middle attack seeks for collisions on the *intermediate chaining values* in iterated hash functions, resulting in a preimage or second preimage for the hash function.

Starting from the initial value `IV`, we compute `2^(n/2)` candidates for `h = cs48_dm(m1, IV)` and save them in a hash table. Then we generate about `2^(n/2)` fixed-points `(h, m2)` and check for a match in the hash table. Note that the value `h` is in general not under the control of the attacker. Due to the birthday paradox, a match is likely to exist and we have constructed an expandable message with complexity of about `2^(n/2+1)`.

If an n-bit hash function has a maximum of `2^k` n-bit blocks in its messages, then this technique takes about `2^(n/2+1)` work to discover `(1, 2^k)`-expandable messages.

Hence, we can find a preimage for the hash function with a complexity of about `2^(n/2+1)` compression function evaluations and memory requirements of `2^(n/2)`. It is possible to reduce the memory requirements.  
We recall here that in our case study `n = 48` so `N = 2^(n/2) = (1 << 24)`.  
It take our implementation 12 seconds to find and expandable message when measured with `time`.

---

We want quick insertion and search for a large number of entries. So we use a hash table : `uthash`. We could use `Packed Radix Tree` for decreasing the memory usage.
We start by initilizing the random generator with a custom seed for reproducibility. Then we build the `m1` message set by calling `h = cs48_dm(m1, IV)` and store `(h, m1)` in the hash table.   
We build the `m2` message set and try to find the fixed point in the hash table. We added a timer to break the search loop after 2 minutes.

For testing the function `find_exp_mess`, we build a message `m = m1 || m2 || ... || m2` and check that the results of `hs48` with message length of 1, 2 and 4 (i.e. 0, 1 and 3 copies of m2) are the same.

### Q2.

Remember we want to construct a preimage for the target message `m` consisting of `t=2^18` message blocks.  

First, we construct an expandable message. This will provide messages over a wide range of lengths and bypass Merkle-Damgard strengthening. The attack can reuse an expandable message as the base for finding second preimages on as many target messages as may be provided.  
Then we carry out the long message attack from the end of that expandable message. To compensate all the message blocks that were skipped by the long message attack, **we expand the expandable message** in order to get a new message of the same length as the target message `m`, resulting in the same hash value. Hence, we get a second preimage for `m`.  

The complexity of the attack depends on the length of the target message and is given by the complexity of the long message attack and the complexity of finding the expandable message.  

Since in most commonly used hash functions the message length is restricted, the attack complexity is **dominated by the cost of the long message attack**, i.e. `2^n/t` compression function evaluations. The memory requirements is `t` values stored in our hash table. It is easy to see that for larger `t`, ie. larger message blocks, the complexity of the attack decreases.  
In our case with `hs48` the maximum message length is about `2^48` blocks. Hence, if we have a target message with `2^18` message blocks, then a second preimage can be found with a complexity of about `2^30` (values tested). Note that a brute force second preimage attack has a complexity of about `2^48`.  
It take our implementation 3 minutes to find a collision when measured with `time`.

## Choose the best hash function in the hash table

With `uthash` you can choose the best hash function.  
First add `-DHASH_EMIT_KEYS=3` to the `Makefile` build options after `$(CC)`.  
Then call the executable and collect the data : `./bin/second_preim_48_fillme 3>second_preim.keys`.  
And finally call `keystats` (found in the [uthash tests directory](https://github.com/troydhanson/uthash/tree/master/tests)).

```bash
./keystats second_preim.keys 
fcn  ideal%     #items   #buckets  dup%  fl   add_usec  find_usec  del-all usec
---  ------ ---------- ---------- -----  -- ---------- ----------  ------------
OAT   76.4%   17039358   16777216   98%  ok    7925688    4954666       1336433
SAX   71.0%   17039358   16777216   98%  ok    9054225    3781487       1163878
SFH   70.8%   17039358   16777216   98%  ok    8189735    2989405       1194189
JEN   70.3%   17039358   16777216   98%  ok    8484610    3615302       1100488
FNV   67.8%   17039358   16777216   98%  ok    9656186    3220381       1126630
BER   66.8%   17039358   16777216   98%  ok    8258472    2954527       1225213
```

`OAT` is the best hash function with `ideal%=76.4%` and `find_usec=4954666`. This is the function that provides the most even distribution for your keys.  
We can add the flag `-DHASH_FUNCTION=HASH_OAT` to our build options. The default hash function is `JEN`.

## Sources

- [Second Preimages on n-bit Hash Functions for Much Less than 2^n Work](https://eprint.iacr.org/2004/304.pdf)
- [Parallel Rho Hash Collisions](https://crypto.stackexchange.com/questions/44955/parallel-rho-hash-collisions)
- [Packed Radix Tree in C](https://github.com/antirez/rax)