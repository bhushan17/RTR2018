#include <stdio.h>
int main(void)
{
	//variable declarations
	int num;

	//code
	printf("\n\n");

	printf("Bhushan Printing Digits 10 to 1 : \n\n");

	num = 10;
	do
	{
		printf("\t%d\n", num);
		num--;
	}while (num >= 1);

	printf("\n\n");

	return(0);
}
