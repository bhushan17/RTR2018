#include <stdio.h>
int main(void)
{
	//variable declarations
	int num1, num2;

	//code
	printf("\n\n");

	num1 = 1;
	while(num1 <= 3)
	{
		printf("num1 = %d\n", num1);
		printf("--------\n\n");
		
		num2 = 1;
		while (num2 <= 2)
		{
			printf("\tnum2 = %d\n", num2);
			num2++;
		}
		num1++;
		printf("\n\n");
	}
	return(0);
}
