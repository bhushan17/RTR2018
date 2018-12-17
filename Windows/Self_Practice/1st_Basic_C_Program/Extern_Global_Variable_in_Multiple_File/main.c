/*##################################################
## 3. Extern Global Variable in Multiple File
##
## Repeat assignment no 1)
## but split the two functions ->
## "change_count_by_one" & "change_count_buy_two" in two different files say File_01.c and File_02.c respectively
##
## NOTE: here "count" variable is declared globally in main file -> eg.main.c
## 		To access it in other two files,
## 		it must first be re-declared as an external variable in the global scope of the files
## 		along with the 'extern' keyword and its proper data type
## 		for eg. extern int global_count;
##
###################################################*/

//****************** Includes header files **************************//
#include<stdio.h>
#include "file_01.h"
#include "file_02.h"
//****************** Declare Global variables **************************//
int count = 5;



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