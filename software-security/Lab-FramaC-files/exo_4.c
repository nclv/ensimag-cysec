

/* Is this code bug-free ? */

// 1. run the VSA ; what do you conclude ?

// 2. Do it better :
//     - find (by hand !)  the post condition of function "compute"
//       and insert this information in the code as an assertion

// 3. Do the best
//     - prove this post condition (using WP)


/*@
  ensures (y >= 0 ==> \result == x + y) && (y < 0 ==> \result == x);
*/
int compute (int x, int y) {

	int z, u;
	u = x ;
	z = 0 ;
	while (z<y) {
			z = z+1 ;
			u = u+1 ;
	};
        return u ;

}

int main() {

   int tab[10] ;

   int a ;

   a = compute (5, 3) ;
   tab[a] = 0 ;

   return 0 ;

}
