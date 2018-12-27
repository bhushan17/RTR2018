/*##################################################
## 1. GLOBAL VARIABLE
##
## take one global "count" variable.
## call two functions from main,
##
##  a.change_count_by_one ->
##  i)increase count variable by one.
##  ii)print variable count in it.
##  
##  b.change_count_buy_two ->
##  i)increase count variable by two.
##  ii)print variable count in it.
##  
##  print count variable before and after these two function call from main.
###################################################*/

//****************** Includes header files **************************//
#include<stdio.h>

//****************** Declare Global variables **************************//
int count = 5;


//****************** Declare function defination **************************//

void change_count_by_one(void);
void change_count_by_two(void);

// ****************** Function defination ********************************//


void change_count_by_one(void)
{
	count = count + 1;
	printf("Value of count in 'change_count_by_one' function = %d\n", count);
}

void change_count_by_two(void)
{
	count = count + 2;
	printf("Value of count in 'change_count_by_two' function = %d\n", count);
}

int main(void)
{
	printf("Value of count in 'main' before calling 'change_count_by_one' function = %d\n", count);
	change_count_by_one();
	printf("Value of count in 'main' after calling 'change_count_by_one' function = %d\n", count);
	printf("Value of count in 'main' before calling 'change_count_by_two' function = %d\n", count);
	change_count_by_two();
	printf("Value of count in 'main' after calling 'change_count_by_two' function = %d\n", count);
	
	return (0);
}