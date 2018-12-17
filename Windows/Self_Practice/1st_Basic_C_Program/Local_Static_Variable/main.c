/*##################################################
## 4. Ordinary Local Variable
##
## initialize one local varible in main say "a" variable with 5.
## print variable a.
## call function -> change_count() from main function 2 times.
##
## a.change_count ->
## 	i)initialize local variable with same name "local_count" with 0.
## 	i)increase local_count variable by one.
## 	ii)print variable local_count in it.
##
## 5.Local Static Variable
##
## Repeat Assignment 4
## the only change is in change_count function
## a. initialize variable as "local static variable" instead of "local variable".
##
###################################################*/

//****************** Includes header files **************************//
#include<stdio.h>

//****************** Declare Global variables **************************//


//****************** Declare function defination **************************//

void change_count(void);


// ****************** Function defination ********************************//


void change_count(void)
{
	static int count = 0;
	count = count + 1;
	printf("Value of count in 'change_count' function = %d\n", count);
}


int main(void)
{
	
	int count = 5;
	printf("Value of count in 'main' before calling 'change_count' function 1st time = %d\n", count);
	change_count();
	printf("Value of count in 'main' after calling 'change_count' function 1st time = %d\n", count);
	
	printf("Value of count in 'main' before calling 'change_count' function 2nd time = %d\n", count);
	change_count();
	printf("Value of count in 'main' after calling 'change_count' function 2nd time = %d\n", count);
	
	return (0);
}