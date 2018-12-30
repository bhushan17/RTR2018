/*##################################################
## Take a integer input N from user and print its fibonacci series till that number.
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
	int maxFibonacciTerm = ZERO, firstTerm = ZERO, secondTerm = ONE, newterm = ZERO, iteration = ZERO;
	
	printf("Enter the fibinacci terms want to print.\n");
	scanf(" %d", &maxFibonacciTerm);

	printf("Fibonacci Series :  %d  %d", firstTerm, secondTerm);

	for (iteration = ONE; iteration <= maxFibonacciTerm; iteration++) {

		newterm = firstTerm + secondTerm;
		printf("  %d", newterm);
		firstTerm = secondTerm;
		secondTerm = newterm;
	}
	
	getch();
	return (0);
}

 