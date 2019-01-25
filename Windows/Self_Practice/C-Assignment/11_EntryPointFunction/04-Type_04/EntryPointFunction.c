/*###################################
##
## Entry Point Function => main()
## Valid Return Type (int)
## and 2 Parameter (int argc, char *argv[])
##
####################################*/
#include<stdio.h>

int main(int argc, char *argv[])
{
	int i;
	
	printf("\n\n");
	printf("Hello World !!!\n\n");
	printf("Number of Command Line Arguments = %d\n\n", argc);
	
	printf("Command Line Arguments Passed To This Program Are : \n\n");
	for(i=0;i<argc;i++)
	{
		printf("Command Line Argument Number %d = %s\n", (i+1),argv[i]);
	}
	printf("\n\n");
	return(0);
}