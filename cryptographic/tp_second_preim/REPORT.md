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