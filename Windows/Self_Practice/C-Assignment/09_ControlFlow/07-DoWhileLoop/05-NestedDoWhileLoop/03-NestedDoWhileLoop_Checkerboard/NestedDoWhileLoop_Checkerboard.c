#include <stdio.h>
int main(void)
{
	//variable declarations
	int outerloop, innerloop, pattern;

	//code
	printf("\n\n");

	outerloop = 0;
	do
	{
		innerloop = 0;
		do
		{
			pattern = ((outerloop & 0x8) == 0) ^ ((innerloop & 0x8) == 0);

			if (pattern == 0)
				printf("  ");

			if (pattern == 1)
				printf("* ");

			innerloop++;

		}while (innerloop < 64);
		printf("\n\n");
		outerloop++;
	}while (outerloop < 64);
	return(0);
}
