#include <stdio.h>
#include <conio.h>
int main(void)
{
	//variable declarations
	int num;
	char ch;

	//code
	printf("\n\n");

	printf("Bhushan Printing Every Numbers From 1 to 100 For Every User Input. Exitting the Loop When User Enters Character 'Q' or 'q' : \n\n");
	printf("Bhushan Enter Character 'Q' or 'q' To Exit Loop : \n\n");

	for (num = 1; num <= 100; num++)
	{
		printf("\t%d\n", num);
		ch = getch();
		if (ch == 'Q' || ch == 'q')
		{
			break;
		}
		printf("Bhushan in LOOP...");
	}

	printf("\n\n");
	printf("Bhushan EXITTING LOOP...");
	printf("\n\n");

	return(0);
}
