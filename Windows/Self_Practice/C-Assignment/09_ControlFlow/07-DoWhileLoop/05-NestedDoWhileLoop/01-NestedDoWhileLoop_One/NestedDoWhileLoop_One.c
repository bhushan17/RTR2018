#include <stdio.h>
int main(void)
{
	//variable declarations
	int outerloop, innerloop;

	//code
	printf("\n\n");

	outerloop = 1;
	do
	{
		printf("outerloop = %d\n", outerloop);
		printf("--------\n\n");
		
		innerloop = 1;
		do
		{
			printf("\tinnerloop = %d\n", innerloop);
			innerloop++;
		}while (innerloop <= 3);
		outerloop++;
		printf("\n\n");
	}while (outerloop <= 3);
	return(0);
}
