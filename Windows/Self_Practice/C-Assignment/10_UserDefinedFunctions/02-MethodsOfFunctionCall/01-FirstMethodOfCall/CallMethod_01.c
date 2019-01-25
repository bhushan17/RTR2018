/*####################
##
##  USER Defined Functions : Method Of Calling Function 1
##	Calling All Functions in main() Directly
##
########################*/

#include<stdio.h>

int main(int argc, char *argv[], char *envp[])
{
	void MyAddition(void);
	int MySubtraction(void);
	void MyMultiplication(int, int);
	int MyDivision(int, int);
	
	//variable declarations
	int result_subtraction;
	int a_multiplication, b_multiplication;
	int a_division, b_division, result_division;
	
	//code
	
	// ***** ADITION ******
	MyAddition();
	
	//****** SUBSTRACTION ************
	result_subtraction = MySubtraction();
	printf("\n\n");
	printf("Subtraction Yields Results = %d\n", result_subtraction);
	
	//**** MULTIPLICATION *******
	printf("\n\n");
	printf("Enter Integer Value For 'A' For Multiplication : ");
	scanf("%d", &a_multiplication);
	
	printf("\n\n");
	printf("Enter Integer Value For 'B' For Multiplication : ");
	scanf("%d", &b_multiplication);
	
	MyMultiplication(a_multiplication, b_multiplication);
	
	// **** DIVISION ******
	printf("\n\n");
	printf("Enter Integer Value For 'A' for Division :");
	scanf("%d", &a_division);
	
	printf("\n\n");
	printf("Enter Integer Value For 'B' for Division :");
	scanf("%d", &b_division);
	
	result_division = MyDivision(a_division, b_division);
	printf("\n\n");
	printf("Division of %d and %d Gives = %d (Quotient)\n", a_division, b_division, result_division);
	
	printf("\n\n");
	
	return(0);	
	
}


//******* Function Defination of MyAddition ()*******
void MyAddition(void)
{
	int a,b,sum;
	
	printf("\n\n");
	printf("Enter Integer value For 'A' or Addition : ");
	scanf("%d", &a);
	
	printf("\n\n");
	printf("Enter Integer Value For 'B' For addition : ");
	scanf("%d", &b);
	
	sum = a+b;
	printf("\n\n");
	printf("Sum of %d And %d = %d\n\n",a,b,sum);
	
}

//******* Function Defination of MySubstraction () *****
int MySubtraction(void)
{
	int a,b,substraction;
	
	//code
	printf("\n\n");
	printf("Enter Integer Value FOR 'A' For Substraction : ");
	scanf("%d", &a);
	
	printf("\n\n");
	printf("Enter Integer Value FOR 'B' For Substraction : ");
	scanf("%d", &b);
	
	substraction = a - b;
	return(substraction);
	
}

// ******* Function Defination of Multiplication() ******
void MyMultiplication(int a, int b)
{
	int multiplication;
	
	multiplication = a * b;
	
	printf("\n\n");
	printf("Multiplication of %d And %d =%d\n\n", a,b, multiplication);
}

// ******** Function Defination of MyDivision() ********
int MyDivision(int a, int b)
{
	int division_quotient;
		
	if(a>b)
		division_quotient = a/b;
	else
		division_quotient = b/a;
	
	return(division_quotient);
	
}