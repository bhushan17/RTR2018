#include <stdio.h>
int main(void)
{
	//variable declarations
	float f_num1;
	float f_num2 = 1.7f; //simply change this value ONLY to get different outputs...

	//code
	printf("\n\n");

	printf("Bhushan Printing Numbers %f to %f : \n\n", f_num2, (f_num2 * 10.0f));

	f_num1 = f_num2;
	do
	{
		printf("\t%f\n", f_num1);
		f_num1 = f_num1 + f_num2;
	}while (f_num1 <= (f_num2 * 10.0f));

	printf("\n\n");

	return(0);
}
