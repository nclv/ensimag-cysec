#include <limits.h>
#include <stdio.h>

int g(int a, int b)
{
  if (a < 0 || b <=0) {
	printf("incorrect arguments \n");
  	return (-1);
  }
  printf("a+b : %i \n", a+b);
  // if (a+b < 0) {
	// printf("overflow error \n");
  // 	return (-1) ;
  // }
  if (a > INT_MAX - b) {
    printf("overflow error \n");
    return (-1);
  }
  printf("no detected error \n");
  return(a+b) ;
};

void main () {
 int r ;
 r = g(4,8);
 r = g(2, INT_MAX);
}
