#include <stdio.h>
int main(void)
{
	//variable declarations
	int start_num, end_num, start;

	//code
	printf("\n\n");

	printf("Bhushan enter An Integer Value From Which Iteration Must Begin : ");
	scanf("%d", &start_num);

	printf("How Many Digits Bhushan You Want To Print From %d Onwards ? : ", start_num);
	scanf("%d", &end_num);

	printf("Bhushan Printing Digits %d to %d : \n\n", start_num, (start_num + end_num));

	for (start = start_num; start <= (start_num + end_num); start++)
	{
		printf("\t%d\n", start);
	}

	printf("\n\n");

	return(0);
}
