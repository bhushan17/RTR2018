#include <stdio.h>
int main(void)
{
	//variable declarations
	int num1, num2, num3;

	//code
	printf("\n\n");
	
	num1 = 1;
	while (num1 <= 2)
	{
		printf("num1 = %d\n", num1);
		printf("--------\n\n");
		
		num2 = 1;
		while (num2 <= 2)
		{
			printf("\tnum2 = %d\n", num2);
			printf("\t--------\n\n");
			
			num3 = 1;
			while (num3 <= 2)
			{
				printf("\t\tnum3 = %d\n", num3);
				num3++;
			}
			printf("\n\n");
			num2++;
		}
		printf("\n\n");
		num1++;
	}
	return(0);
}
