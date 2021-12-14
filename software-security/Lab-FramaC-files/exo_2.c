
/*

  - This code may contain arithmetic overflows ...
     Run the RTE analysis to see why:
        frama-c-gui -rte exo_2.c

  - Try to correct them (without changing the read function) but by adding 
     conditional statements before each arithmetic expressions 

  - Check with VSA is your result is correct: 
        frama-c-gui -rte -eva exo_2.c

  - Additionally you can also try WP:  
        frama-c-gui -rte -wp exo_2.c
    


*/



#include <limits.h>
  
int read();
// no body, may return any int value

int main()
{ int a=read();
  int b=read();
  unsigned int r;

  // OK with -wp, not OK with -eva (need wp)
  if (a >= 0 ? b > INT_MAX - a : b < INT_MIN - a) {
    return -1;
  }

// OK with -wp, not OK with -eva
//   if (((b > 0) && (a > (INT_MAX - b))) || ((b < 0) && (a < (INT_MIN - b)))) {
//     return -1;
//   }
  
  r = a+b;
  
  // OK with -wp, not OK with -eva (OK with higher slevel)
  if (a >= 0 ? a > INT_MAX / 2 : a < INT_MIN / 2) {
    return -1;
  }

  r = a*2;
  
  return 0 ;
}
