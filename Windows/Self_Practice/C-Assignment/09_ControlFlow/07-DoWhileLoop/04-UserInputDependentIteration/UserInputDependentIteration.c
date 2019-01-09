#include <stdio.h>
int main(void)
{
	//variable declarations
	int start_num, num_count, increment;

	//code
	printf("\n\n");

	printf("Bhushan Enter An Integer Value From Which Iteration Must Begin : ");
	scanf("%d", &start_num);

	printf("Bhushan How Many Digits Do You Want To Print From %d Onwards ? : ", start_num);
	scanf("%d", &num_count);

	printf("Printing Digits %d to %d : \n\n", start_num, (start_num + num_count));

	increment = start_num;
	do
	{
		printf("\t%d\n", increment);
		increment++;
	}while (increment <= (start_num + num_count));

	printf("\n\n");

	return(0);
}
