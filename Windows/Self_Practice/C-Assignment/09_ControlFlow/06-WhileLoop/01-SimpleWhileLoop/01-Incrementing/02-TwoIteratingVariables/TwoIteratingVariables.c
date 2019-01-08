#include <stdio.h>
int main(void)
{
	//variable declarations
	int num1, num2;

	//code
	printf("\n\n");

	printf("Bhushan Printing Digits 1 to 10 and 10 to 100: \n\n");

	num1 = 1;
	num2 = 10;
	while (num1 <= 10, num2 <= 100)
	{
		printf("\t %d \t %d\n", num1, num2);
		num1++;
		num2 = num2 + 10;
	}

	printf("\n\n");

	return(0);
}
