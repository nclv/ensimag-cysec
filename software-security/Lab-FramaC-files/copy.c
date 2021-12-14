/*@ requires \valid(src+(0..99)) 
          && \valid(dst+(0..99)) 
          && \separated(src+(0..99), dst+(0..99));
 @ ensures (\forall  int k; 0 <=  k < 100 ==>  dst[k]==\old(src[k])) ;
 @ assigns dst[0..99] ;
@*/

 // @ ensures (\forall  int k; 0 <=  k < 100 ==>  dst[k]==\old(src[k]);
// 
void copy (int* dst, int *src)
{  int i=0;
 /*@ loop invariant 0<= i <= 100 &&
    @    (\forall int k; 0 <= k < i ==> dst[k]==\at(src[k], Pre)) ;
    @ loop assigns dst[0..99], i;
    @*/

   while (i<100) {dst[i]=src[i] ;  i++; }
}
