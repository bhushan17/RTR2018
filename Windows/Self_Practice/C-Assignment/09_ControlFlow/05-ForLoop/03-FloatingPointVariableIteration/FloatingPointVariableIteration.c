#include <stdio.h>
int main(void)
{
	//variable declarations
	float bhushan_f;
	float bhushan_f_num = 1.7f; //simply change this value ONLY to get different outputs...

	//code
	printf("\n\n");

	printf("Bhushan Printing Numbers %f to %f : \n\n", bhushan_f_num, (bhushan_f_num * 10.0f));

	for (bhushan_f = bhushan_f_num; bhushan_f <= (bhushan_f_num * 10.0f); bhushan_f = bhushan_f + bhushan_f_num)
	{
		printf("\t%f\n", bhushan_f);
	}

	printf("\n\n");

	return(0);
}
