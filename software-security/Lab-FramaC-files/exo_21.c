
/*

 - Run a VSA analysis (by hand) to check if this code is bug free ...

 - Confirm your result with frama-c  ...

 - Intuitively, is this code really bug free or not ???

 - Run WP analysis to get a more complete verdict on this example 

 - Can you explain theses results ?

*/

// /*@ assert rte: index_bound: 0 ≤ (int)(b - a); */
// /*@ assert rte: index_bound: (int)(b - a) < 10; */
// /*@ assert rte: signed_overflow: -2147483648 ≤ b - a; green */ 
// /*@ assert rte: signed_overflow: b - a ≤ 2147483647; */

// /*@ assert rte: index_bound: 0 ≤ (int)(a - b); */
// /*@ assert rte: index_bound: (int)(a - b) < 10; */
// /*@ assert rte: signed_overflow: -2147483648 ≤ a - b; green */
// /*@ assert rte: signed_overflow: a - b ≤ 2147483647; */

// add 0 <= \result to read() to all green -wp, all green except first -eva


//@ ensures 0 <= \result <10 ;
int read();
// no body, may return any int value

void main()
{ int a=read();
  int b=read();
  int t[10] ;
  if (a < b)
	t[b-a] = 0 ;
  else 
	t[a-b] = 0 ;
}
