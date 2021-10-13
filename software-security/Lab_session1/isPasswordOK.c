
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int IsPasswordOK(void)
{
	char Password[12];
	gets(Password);
	return 0 == strcmp(Password, "goodpass");
}

int main(void)
{
	int status1, status2;
	puts("Enter your password twice");
	status1 = IsPasswordOK();
	status2 = IsPasswordOK();
	if (status1 == 0)
		puts("Access denied");
	else
		puts("OK");
	if (status2 == 0)
		puts("Access denied");
	else
		puts("OK");
}
