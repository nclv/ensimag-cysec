

// This pgm expects 2 integer arguments 
// Find the winning values ....


#include <stdio.h>
#include <stdlib.h>

int main (int argc, char *argv[]) 
{ 
  char x=0 ;
  char t1[8] ;
  int i;
  printf("address of x: %p\n", &x);
  printf("address of t1: %p\n", t1);

  for (i=0;i<=atoi(argv[2]);i++) {
    printf("Before modification - %d : %p: %d\n", i, &t1[i], t1[i]);
	  t1[i]= atoi(argv[1]) ;
    printf("%d : %p: %d\n", i, &t1[i], t1[i]);
  }

  printf("%p: %d\n", &x, x);
  if (x != 0) 
	  printf("You win !\n") ;
  else 
	  printf("You loose ...\n") ;
  return 0 ;
}
