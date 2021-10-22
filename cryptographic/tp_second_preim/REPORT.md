# Second pre-image attack

h_l = H(m_1||...||m_l)

**Obj. :** find M' such that H(M') = h_l.  
If M'' such that H(M'') = h_i then H(M''||m_{i+1}||...||m_l) = h_l  
Now the obj. is to find M'' such that there exist i verifying the previous equality.  

Instead of having a single target we now have l targets : 2^n/l.

If the (final) padding doesn't match we don't have an equality anymore.  
So we use extendable messages
