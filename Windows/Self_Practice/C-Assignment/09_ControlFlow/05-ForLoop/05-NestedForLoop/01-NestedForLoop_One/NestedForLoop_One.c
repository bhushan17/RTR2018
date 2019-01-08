#include <stdio.h>
int main(void)
{
	//variable declarations
	int num1, num2;

	//code
	printf("\n\n");
	for (num1 = 1; num1 <= 3; num1++)
	{
		printf("num1 = %d\n", num1);
		printf("--------\n\n");
		for (num2 = 1; num2 <= 5; num2++)
		{
			printf("\tnum2 = %d\n", num2);
		}
		printf("\n\n");
	}
	return(0);
}
