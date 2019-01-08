#include <stdio.h>
int main(void)
{
	//variable declarations
	int num1, num2, num3;

	//code
	printf("\n\n");
	for (num1 = 1; num1 <= 2; num1++)
	{
		printf("num1 = %d\n", num1);
		printf("--------\n\n");
		for (num2 = 1; num2 <= 2; num2++)
		{
			printf("\tnum2 = %d\n", num2);
			printf("\t--------\n\n");
			for (num3 = 1; num3 <= 3; num3++)
			{
				printf("\t\tnum3 = %d\n", num3);
			}
			printf("\n\n");
		}
		printf("\n\n");
	}
	return(0);
}
