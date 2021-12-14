

/*   
   - Is this pgm bug free ?
   - run a VSA (with intervals) by hand to convince yourself ...
       what is the set of values for x after the loop ?
   - run frama-c with options RTE and VSA analysis to check the result :
         frama-c-gui -rte -eva exo_1.c
   - give the value 10000 to constant N and run VSA again ... 
   - give the value 10001 to constant N and run VSA again ... 
      Conclusion ?
*/

#define N 1001

int main() {

  int x ;
  int tab[N+1] ;

  x=0;
  while (x<N) 
	x=x+2;
  tab[x] = 0 ;
  return 0 ;

}
