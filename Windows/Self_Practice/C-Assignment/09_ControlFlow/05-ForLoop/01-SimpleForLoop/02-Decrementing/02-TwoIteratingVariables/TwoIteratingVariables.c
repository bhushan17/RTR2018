#include <stdio.h>
int main(void)
{
	//variable declarations
	int Bhushan_num1, Bhushan_num2;

	//code
	printf("\n\n");

	printf("Bhushan Printing Digits 10 to 1 and 100 to 10 : \n\n");

	for (Bhushan_num1 = 10, Bhushan_num2 = 100; Bhushan_num1 >= 1, Bhushan_num2 >= 10; Bhushan_num1--, Bhushan_num2 -= 10)
	{
		printf("\t %d \t %d\n", Bhushan_num1, Bhushan_num2);
	}

	printf("\n\n");

	return(0);
}
