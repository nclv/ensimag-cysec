/*@ requires \valid(dst+(0..99)) && \valid(src+(0..99)) ;
@ ensures (\forall  int k; 0 <=  k < 100 ==>  dst[k]==\old(src[k])) ;
 @ assigns dst[0..99], src[0..99] ;

@*/

void copy (int* dst, int *src)
{  int i;
if (src < dst)
   { i=99; 
/*@ loop invariant -1<= i <= 99 ;
    @ loop invariant    i==i ;
    @
    @ loop assigns dst[0..99], i, src[0..99];
    @*/
   while (i>=0) {dst[i]=src[i] ; i--;};}
else 
   { i=0; 
  /*@ loop invariant 0<= i <= 100 ;
    @ loop invariant    i==i ;
    @ 
    @ loop assigns dst[0..99], i, src[0..99];
    @*/

    while (i< 100) {dst[i]=src[i] ; i++;}; }
}

