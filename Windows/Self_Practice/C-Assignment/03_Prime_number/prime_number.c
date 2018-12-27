/*##################################################
## Take a integer input from user and print all 
## prime number till that integer.
## constraints 
## 1<=N<=1000
###################################################*/

//****************** Includes header files **************************//
#include<stdio.h>
#include"Stdtype.h"
#include<stdbool.h>

//****************** Declare Global variables **************************//

//****************** Function Declaration ******************************// 

void PrintPrimeNumber(int);

//******* Entry Function ****************//
int main()
{
	int primeNumberList = ZERO;
	
	do
	{
		printf("Enter the interger number to get Prime number list till that number.\n");
		scanf("\t %d", &primeNumberList);
		if ((MIN <= primeNumberList) && (primeNumberList <= MAX)) {
			PrintPrimeNumber(primeNumberList);
		}
		else {
			printf("Entered number is not in range of 1 to 1000.\n");
		}
	} while (primeNumberList < MIN || primeNumberList > MAX);
	
	getch();
	return (0);
}

void PrintPrimeNumber(int numberList)
{
	int numberInList = ZERO, devicer = ZERO;
	bool isPrime = false;
	if (ONE == numberList)
		printf("1 is not prime number.");
	else {
		printf("Prime number list :");
		for (numberInList = TWO; numberInList <= numberList; numberInList++) {
			isPrime = true;
			for (devicer = TWO; devicer <= numberInList / TWO; devicer++) {
				if (numberInList%devicer == ZERO) {
					isPrime = false;
				}
			}
			if (isPrime)
				printf("  %d", numberInList);
		}
	}
}