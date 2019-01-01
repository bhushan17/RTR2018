/*##################################################
## Convert all alphabets from user input string, lowercase to uppercase and viceversa.
## 
## ###################################################*/

//****************** Includes header files **************************//
#include<stdio.h>
#include"Stdtype.h"
#include<string.h>


//****************** Declare Global variables **************************//

//****************** Function Declaration ******************************// 


//******* Entry Function ****************//
int main()
{
	int strLen = ZERO, index = ZERO;
	char inputString[100] = { ZERO };
	printf("Enter the input string.\n");
	scanf(" %[^\n]s", &inputString);
	strLen = strlen(inputString);
	
	if ((ONE <= strLen) && (strLen <= HUNDRED))
	{
		while (inputString[index] != '\0')
		{
			if (inputString[index] >= 'a' && inputString[index] <= 'z')
				inputString[index] = inputString[index] - 32;
			else if (inputString[index] >= 'A' && inputString[index] <= 'Z')
				inputString[index] = inputString[index] + 32;

			index++;
		}
	}
	else
		printf("Enter the string having length in between 1 to 100.\n");

	printf("Converted output String : \n%s", inputString);


		
	getch();
	return (0);
}

 