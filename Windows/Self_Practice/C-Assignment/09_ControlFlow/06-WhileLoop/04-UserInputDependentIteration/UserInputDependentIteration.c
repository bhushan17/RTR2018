#include <stdio.h>
int main(void)
{
	//variable declarations
	int num1, num2, num3;

	//code
	printf("\n\n");

	printf("Bhushan Enter An Integer Value From Which Iteration Must Begin : ");
	scanf("%d", &num1);

	printf("Bhushan How Many Digits Do You Want To Print From %d Onwards ? : ", num1);
	scanf("%d", &num2);

	printf("Bhushan Printing Digits %d to %d : \n\n", num1, (num1 + num2));

	num3 = num1;
	while (num3 <= (num1 + num2))
	{
		printf("\t%d\n", num3);
		num3++;
	}

	printf("\n\n");

	return(0);
}
