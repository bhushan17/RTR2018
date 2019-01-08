#include <stdio.h>
int main(void)
{
	//variable declarations
	int num1, num2, num3;

	//code
	printf("\n\n");

	num1 = 0;
	while (num1 < 64)
	{
		num2 = 0;
		while (num2 < 64)
		{
			num3 = ((num1 & 0x8) == 0) ^ ((num2 & 0x8) == 0);

			if (num3 == 0)
				printf("  ");

			if (num3 == 1)
				printf("* ");

			num2++;

		}
		printf("\n\n");
		num1++;
	}
	return(0);
}
