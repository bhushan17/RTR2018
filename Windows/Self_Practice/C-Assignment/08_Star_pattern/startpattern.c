/*##################################################
## Convert all alphabets from user input string, lowercase to uppercase and viceversa.
## 
## ###################################################*/

//****************** Includes header files **************************//
#include<stdio.h>
#include"Stdtype.h"



//****************** Declare Global variables **************************//

//****************** Function Declaration ******************************// 


//******* Entry Function ****************//
int main()
{
	int row = ZERO, colum = ZERO, max = 5, colum1 = ZERO, colum2 = ZERO, num = ZERO;
	int matrix[9][9];
	
	printf("1. Right Triangle Star Pattern\n");

	for (row = ONE; row <= max; row++)
	{
		printf("\t");
		for (colum = ONE; colum <= max; colum++)
		{
			if (colum <= row)
				printf(" *");
		}
		printf("\n");
	}

	printf("2. Mirrored Right Triangle Star Pattern.\n");

	for (row = ONE; row <= max; row++)
	{
		printf("\t");
		for (colum = max; colum >= ONE; colum--)
		{
			if (colum <= row) {
				printf(" *");
			}
				
			else {
				printf("  ");
			}
			
		}
		printf("\n");
	}

	printf("3. Inverted Right Triangle Star Pattern.\n");

	for (row = max; row >= ONE; row--)
	{
		printf("\t");
		for (colum = row; colum >= ONE; colum--)
		{
				printf(" *");
		
		}
		printf("\n");
	}


	printf("4. Inverted Mirrored Right Triangle Star Pattern.\n");

	for (row = ONE; row <= max; row++)
	{
		printf("\t");
		for (colum = ONE; colum <= max; colum++)
		{
			if (colum < row)
				printf("  ");
			else {
				printf("* ");
			}
		}
		printf("\n");
	}

	printf("5. Number Pattern 1.\n");

	for (row = ONE; row <= max; row++)
	{
		printf("\t");
		for (colum = ONE, colum1=(2*max); colum <= (2*max),colum1 >= ONE; colum++, colum1--)
		{
			if (colum <= row) {
				printf("%d", colum);
				num = colum;
			}
			else if(colum!=(row+1)){
				printf(" ");
			}
			
			if (colum1 <= row) {
				printf("%d", num--);
			}
			else {
				printf(" ");
			}
			
		}
		printf("\n");
		
	}

	printf("6. Number Pattern 2.\n");
	/*
	for (row = ONE; row <= max ; row++)
	{
		printf("\t");
		num = max;
		for (colum = ONE, colum1 = ((2 * max) - 1); colum <= max, colum1 >= ONE; colum++, colum1--)
		{
			if (colum <= row) {
				printf(" %d", num--);

			}
			else if (colum1 <= row && colum != colum1) {
				printf(" %d", ++num);
			}
			else {
				printf(" %d", ++num);
				num--;
			}
			if (row == colum && row == max) {
				num++;
			}
			
		}
		printf("\n");

	}

	for (row = ZERO; row < max; row++)
	{
		printf("\t");
		num = max;
		for (colum = max-row, colum1 = ONE; colum >= ONE, colum1 < 2*max; colum--, colum1++)
		{
			if (colum >= ONE && num!=1)
			{
				printf(" %d", num--);
			}
			else if (colum >= ONE && num == 1) {
				printf(" %d", ++num);
			}
			else if (colum1 >= max+row) {
				printf(" %d",num++);

			}
			else {
				printf("  ");
			}

			if (colum >= ONE && num == 1)
				num++;
			
			
			
		
		}
		printf("\n");
	}*/

	// First upper half of the pattern
	for (row = max; row >= 1; row--)
	{
		printf("\t");
		// First inner part of upper half
		for (colum = max; colum > row; colum--)
		{
			printf(" %d", colum);
		}

		// Second inner part of upper half
		for (colum = 1; colum <= (row * 2 - 1); colum++)
		{
			printf(" %d", row);
		}

		// Third inner part of upper half
		for (colum = row + 1; colum <= max; colum++)
		{
			printf(" %d", colum);
		}

		printf("\n");
	}

	// Second lower half of the pattern
	for (row = 1; row < max; row++)
	{
		printf("\t");
		// First inner part of lower half
		for (colum = max; colum > row; colum--)
		{
			printf(" %d", colum);
		}

		// Second inner part of lower half
		for (colum = 1; colum <= (row * 2 - 1); colum++)
		{
			printf(" %d", row + 1);
		}

		// Third inner part of lower half
		for (colum = row + 1; colum <= max; colum++)
		{
			printf(" %d", colum);
		}

		printf("\n");
	}
	getch();
	return (0);
}

 