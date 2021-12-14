



/* Demo 1 : division by 0 (version 1)  */
 
int main() {

  int x ;
  int y ;

  x = 10 ;
  y = 8 ;
  if (x>y) 
	y=y-8;
  else
	y=y+8;
  x = x / y ;
  return 0 ;

}
