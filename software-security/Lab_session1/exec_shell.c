
// This pgm expects a dirname as arguments and prints its contents
// ex : ./a.out /tmp

// If the dirname is too long an error occur and a message can 
// be provided from the keyboard


// question : how to make this pgm open a shell (e.g., /bin/sh ?)


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *p ;

int f (char a[]) 
{ 
  p =( char *) malloc (16 * sizeof(char));  // allocation 1
  strcpy (p, "ls ");
  if (strlen(a) > 14) {
	    printf("filename too long !\n") ;
        free (p);
		return 0 ;
  } else 
  	  strcat (p, a); // build the command 
  return 1 ;
}


int main(int argc, char *argv[]) {
	char *p3 ;
	if (!f(argv[1])) {
    	printf("Error ! Enter your log message (< 24 characters)\n");
    	p3 = (char *) malloc (24 * sizeof (char));  // allocation 2
    	scanf("%s", p3) ;
	} ;
    system (p) ;
	return 0 ;
}

