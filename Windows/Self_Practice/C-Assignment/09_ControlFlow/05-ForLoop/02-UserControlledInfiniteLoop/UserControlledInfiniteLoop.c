#include <stdio.h>
int main(void)
{
	//variable declarations
	char option, ch = '\0';

	//code
	printf("\n\n");
	printf("Once The Bhushan's Infinite Loop Begins, Enter 'Q' or 'q' To Quit The Bhushan's Infinite For Loop : \n\n");
	printf("Bhushan Enter 'Y' oy 'y' To Initiate User Controlled Infinite Loop : ");
	printf("\n\n");
	option = getch();
	if (option == 'Y' || option == 'y')
	{
		for (;;) //Infinite Loop
		{
			printf("Bhushan is in Loop...\n");
			ch = getch();
			if (ch == 'Q' || ch == 'q')
				break; //User Controlled Exitting From Infinite Loop
		}
	}

	printf("\n\n");
	printf("EXITTING Bhushan USER CONTROLLED INFINITE LOOP...");
	printf("\n\n");

	return(0);
}
