/*##################################################
## 2. Extern Global Variable in Single File
##
## initialize global variable eg. "global_count" (integer type) below main function
## declare "global_count" in main
## eg. extern int global_count; -> Since, it is declared after main(),
## 								it must be first re-declared in main() as an external global variable
##
## call below function from main,
## a.change_count ->
##
## 	i)increase count variable by one.
## 	ii)print variable count in it.
##
## print count variable before and after this function call from main.
###################################################*/

//****************** Includes header files **************************//
#include<stdio.h>




//****************** Declare function defination **************************//

void change_count(void);


int main(void)
{
	extern int count;
	printf("Value of count in 'main' before calling 'change_count' function = %d\n", count);
	change_count();
	printf("Value of count in 'main' after calling 'change_count' function = %d\n", count);
	getch();
	return (0);
}


//****************** Declare Global variables **************************//
int count = 5;


// ****************** Function defination ********************************//


void change_count(void)
{
	count = count + 1;
	printf("Value of count in 'change_count' function = %d\n", count);
}

