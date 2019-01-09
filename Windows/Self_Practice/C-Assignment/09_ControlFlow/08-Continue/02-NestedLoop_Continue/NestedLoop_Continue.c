#include <stdio.h>
int main(void)
{
	//variable declarations
	int odd_num, even_num;

	//code
	printf("\n\n");

	printf("Bhushan Outer Loop Prints Odd Numbers Between 1 and 10. \n\n");
	printf("Bhushan Inner Loop Prints Even Numbers Between 1 and 10 For Every Odd Number Printed By Outer Loop. \n\n");

	// condition for a number to be even number => division of number by 2 leaves no remainder (remainder = 0)
	// condition for a number to be odd number => division of number by 2 leaves remainder (remainder = 1 (usually))

	for (odd_num = 1; odd_num <= 10; odd_num++)
	{
		if (odd_num % 2 != 0) //If Number (odd_num) Is Odd..
		{
			printf("odd_num = %d\n", odd_num);
			printf("---------\n");
			for (even_num = 1; even_num <= 10; even_num++)
			{
				if (even_num % 2 == 0) //If Number (even_num) Is Even...
				{
					printf("\teven_num = %d\n",even_num);
				}
				else //If Number (even_num) Is Odd..
				{
					continue;
				}
			}
			printf("\n\n");
		}
		else //If Number (odd_num) Is Even...
		{
			continue;
		}
	}

	printf("\n\n");

	return(0);
}
