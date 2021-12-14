



/* Demo 1 : division by 0 (version 2)  */

//@ ensures \result > 8 ; 
// restricts the set of return values for read()
// may return any int value strictly larger than 8
int read() ;
// no body 
 
int main() {

  int x ;
  int y ;

  x = 10 ;
  y = read() ;  // read a value for y
  if (x>y) 
	y=y-8;
  else
	y=y+8;
  x = x / y ;
  return 0 ;

}
