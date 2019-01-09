#include <stdio.h>
int main(void)
{
	//variable declarations
	int number;

	//code
	printf("\n\n");

	printf("Bhushan Printing Even Numbers From 0 to 50: \n\n");

	for (number = 0; number <= 50; number++)
	{
		// condition for a nuber to be even number => division of number by 2 leaves no remainder (remainder = 0)
		// if remainder is not 0, the number is odd number...
		if (number % 2 != 0) 
		{
			continue;
		}
		else
		{
			printf("\t%d\n", number);
		}
	}

	printf("\n\n");

	return(0);
}
