#include <stdio.h>
int main(void)
{
	//variable declarations
	char option, ch = '\0';

	//code
	printf("\n\n");
	printf("Bhushan Once The Infinite Loop Begins, Enter 'Q' or 'q' To Quit The Infinite For Loop : \n\n");

	do
	{
		do
		{
			printf("\n");
			printf("Bhushan you are in Loop...\n");
			ch = getch(); //control flow waits for character input...
		} while (ch != 'Q' && ch != 'q');
		printf("\n\n");
		printf("Bhushan EXITTING USER CONTROLLED INFINITE LOOP...");

		printf("\n\n");
		printf("Bhushan DO YOU WANT TO BEGIN USER CONTROLLED INFINITE LOOP AGAIN?...(Y/y - Yes, Any Other Key - No) : ");
		option = getch();
	}while (option == 'Y' || option == 'y');

	return(0);
}
