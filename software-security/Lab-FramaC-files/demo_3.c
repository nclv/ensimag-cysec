
#include <limits.h>


/* Demo 1 : division by 0 (version 3)  */

//@ ensures \result < INT_MAX-8 && \result >= 8 ;
// restricts the set of return values for read()
// may return any int value in [8,INT_MAX -8[
int read() ;
// no body 
 
int main() {

  int x ;
  int y ;

  x = 10 ;
  y = read() ;  // read a value for y
  if (y == 8 || y == -8)  return 1 ;  // error case
  else {
  if (x>y) 
	y=y-8;
  else
	y=y+8;
  x = x / y ;
  return 0 ;
  }

}
