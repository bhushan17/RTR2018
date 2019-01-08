#include <stdio.h>
int main(void)
{
	//variable declarations
	int bhushan_num1, bhushan_num2;

	//code
	printf("\n\n");

	printf("Printing Digits 1 to 10 and 10 to 100 : \n\n");

	for (bhushan_num1 = 1, bhushan_num2 = 10; bhushan_num1 <= 10, bhushan_num2 <= 100; bhushan_num1++, bhushan_num2 = bhushan_num2 + 10)
	{
		printf("\t %d \t %d\n", bhushan_num1, bhushan_num2);
	}

	printf("\n\n");

	return(0);
}
