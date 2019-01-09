#include <stdio.h>
int main(void)
{
	//variable declarations
	int num1, num2;

	//code
	printf("\n\n");

	printf("Bhushan Printing Digits 10 to 1 and 100 to 10: \n\n");

	num1 = 10;
	num2 = 100;
	do
	{
		printf("\t %d \t %d\n", num1, num2);
		num1--;
		num2 = num2 - 10;
	}while (num1 >= 1, num2 >= 10);

	printf("\n\n");

	return(0);
}
