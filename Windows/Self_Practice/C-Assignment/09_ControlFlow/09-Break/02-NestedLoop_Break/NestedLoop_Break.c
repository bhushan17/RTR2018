#include <stdio.h>
#include <conio.h>
int main(void)
{
	//variable declarations
	int outer_loop, inner_loop;

	//code
	printf("\n\n");

	for (outer_loop = 1; outer_loop <= 20; outer_loop++)
	{
		for (inner_loop = 1; inner_loop <= 20; inner_loop++)
		{
			if (inner_loop > outer_loop)
			{
				break;
			}
			else
			{
				printf("* ");
			}
		}
		printf("\n");
	}
	printf("\n\n");
	return(0);
}
