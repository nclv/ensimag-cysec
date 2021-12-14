

/*   Find the error and correct it */

// 0. Is there a BoF ?

// 1. Run the VSA and explain the final value of variable x ...
//     Compare this result with the "real" final value of x.

// 2. Try to prove that this code is bug free using WP
//     (you will have to add some loop invariant to help Frama-C) 


int main() {

  int x ;
  int tab[33] ;

  x=1;
  // OK -eva with slevel
  /*@ loop invariant 1 <= x < 33;
      loop assigns x;
  */ 
  while (x<24) 
	x=x*2;
  tab[x] = 0 ;
  return x ;

}
