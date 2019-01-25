/*####################
##
##  User Defined Function : Method of calling function 2
##	Calling Only two function directly in main()
##	rest of the function Trace their call indirectly to main()
##
########################*/

#include<stdio.h>

int main(int argc, char *argv[], char *envp[])
{
	//function prototypes
	void display_information(void);
	void Function_Country(void);
	
	//code
	display_information();
	Function_Country();
	
	return(0);
}

void display_information(void)
{
	// Function prototypes
	void Function_My(void);
	void Function_Name(void);
	void Function_Is(void);
	void Function_FirstName(void);
	void Function_MiddleName(void);
	void Function_Surname(void);
	void Function_OfAMC(void);
	
	//code
	
	// **** Function Calls ******
	Function_My();
	Function_Name();
	Function_Is();
	Function_FirstName();
	Function_MiddleName();
	Function_Surname();
	Function_OfAMC();
	
}

void Function_My(void)
{
	printf("\n\n");
	printf("\tMy");
}

void Function_Name(void)
{
	printf("\n\n");
	printf("\tName");
}

void Function_Is(void)
{
	printf("\n\n");
	printf("\tIs");
}

void Function_FirstName(void)
{
	printf("\n\n");
	printf("\tBhushan");
}

void Function_MiddleName(void)
{
	printf("\n\n");
	printf("\tRajendra");
}

void Function_Surname(void)
{
	printf("\n\n");
	printf("\tKalamkar");
}

void Function_OfAMC(void)
{
	printf("\n\n");
	printf("\tOf ASTROMEDICOMP");
}

void Function_Country(void)
{
	printf("\n\n");
	printf("\tI live in India.");
	printf("\n\n");
}

