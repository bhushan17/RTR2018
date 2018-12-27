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

void Factorial(int);

//******* Entry Function ****************//
int main()
{
	int number = ZERO;
	
	do
	{
		printf("Enter the interger number to its factorial.\n");
		scanf("\t %d", &number);
		if ((MIN <= number) && (number <= MAX)) {
			Factorial(number);
		}
		else {
			printf("Entered number is not in range of 1 to 10.\n");
		}
	} while (number < MIN || number > MAX);
	
	getch();
	return (0);
}

void Factorial(int factorialNumber)
{
	static int result = ONE;
	if (ONE == factorialNumber) {
		printf("Factorial is = %d ", result);
	}
	else {
		result = result * factorialNumber--;
		Factorial(factorialNumber);
	}
			
}




 