/*##################################################
## Take a integer input N from user and print its factorial.
## constraints 
## 1<=N<=1000
###################################################*/

//****************** Includes header files **************************//
#include<stdio.h>
#include"Stdtype.h"
#include<stdbool.h>

//****************** Declare Global variables **************************//

//****************** Function Declaration ******************************// 

int Factorial(int);

//******* Entry Function ****************//
int main()
{
	int number = ZERO;
	
	do
	{
		printf("Enter the interger number to its factorial.\n");
		scanf("\t %d", &number);
		if ((MIN <= number) && (number <= MAX)) {
			printf("Factorial of %d is = %d ", number ,Factorial(number));
		}
		else {
			printf("Entered number is not in range of 1 to 10.\n");
		}
	} while (number < MIN || number > MAX);
	
	getch();
	return (0);
}

int Factorial(int factorialNumber)
{
	
	if (factorialNumber >= ONE ) {		
		return (factorialNumber * Factorial(factorialNumber - 1));		
	}
	else {
		return (1);
	}			
}




 