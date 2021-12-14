#include <assert.h>

void f (int i) {
// OK
   int T[10] ;
   assert(i < 10 && i>=0) ;  
   T[i] = 0 ;
}

void g (int i) {
// Rejected by PC, no test cases generated ...
   int T[10] ;
   T[i] = 0 ;
}
 
void h (int i) {
// may not necessarily find the bug ...
   int T[10] ;
   if (i<11)
      T[i] = 0 ;
}
 
void k (int i) {
// add assertions to tell PathCrawler to try to trigger the bug 
   int T[10] ;
    assert(i<0 || i>=10) ;
      T[i] = 0 ;
}
