# Second pre-image attack

## Notes

h_l = H(m_1||...||m_l)

**Obj. :** find M' such that H(M') = h_l.  
If M'' such that H(M'') = h_i then H(M''||m_{i+1}||...||m_l) = h_l  
Now the obj. is to find M'' such that there exist i verifying the previous equality.  

Instead of having a single target we now have l targets : 2^n/l.

If the (final) padding doesn't match we don't have an equality anymore.  
So we use expandable messages

## Compilation

Use the `Makefile`.

## Part one : Preparatory work

- [Speck block ciphers (p.14)](https://eprint.iacr.org/2013/404.pdf)

### Q1.

We look at the speck parameter table. In our case the block size is 48, the key size is 96, the word size is 24, there are 4 key words, rot alpha is equal to 8, rot beta is equal to 3 and there are 23 rounds.

### Q2.
### Q3.
### Q4.

## Part two : the attack
### Q1.
The input/output chaining value is given on the 48 low bits. So 2^(48/2) = 2^24 compression function calls.

Generate about 2^(n/2) fixed-points (H1, M2) and save them in a list L. Note that the value H1 isin general not under the control of the attacker. Now, starting from the initial value H0 we compute 2n/2 candidates for H1=f(H0,M1) and check for a match in the listL. Due to the birthday paradox a match is likely to exist and we have constructed an expandable message with complexity of about 2^(n/2)+1.

### Q2.

Remember we want to construct a pre-image for the target message M consisting of t message blocks.
First, we construct an (a,b)-expandable message (with b being approximately equal to t) using one of the methods described before. This will provide messages over a wide range of lengths. 
Then we carry out the long message attack from the end of that expandable message. To compensate all the message blocks that were skipped by the long message attack, we expand the expandable message in order to get a new message of the same length as th etarget message M, resulting in the same hash value. Hence, we get a second pre-image for M. 
The complexity of the attack depends on the length of the target message and is given by the complexity of the long message attack and the complexity of finding the expandable message. 
Since in most commonly used hash functions the message length is restricted, the attack complexity is dominated by the cost of the long message attack, i.e. 2^n/t compression function evaluations.
As an example, in SHA-256 the maximum message length is about 255 blocks. Hence, if we have a target message with 255 message blocks, then a second pre-image can be found with a complexity of about 2^201. Note that a brute force second pre-image attack has a complexity of about 2^256.

## Sources

- [Second Preimages on n-bit Hash Functions for Much Less than 2^n Work](https://eprint.iacr.org/2004/304.pdf)
- [Parallel Rho Hash Collisions](https://crypto.stackexchange.com/questions/44955/parallel-rho-hash-collisions)
- [Packed Radix Tree in C](https://github.com/antirez/rax)