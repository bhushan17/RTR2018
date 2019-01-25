/*##################################
##
##Entry Point Function => main() => Valid Return Type (int)
## And 3 Parameter (int argc, char *argv[], char *envp[])
##
####################################*/

#include<stdio.h>

int main(int argc, char *argv[], char *envp[])
{
	int i;
	
	printf("\n\n");
	printf("Hello World !!!\n\n");
	
	printf("Number of Command Line Arguments = %d\n\n",argc);
	
	printf("Command line Argument Passed To This Program Are : \n\n");
	for(i=0;i<argc;i++)
	{
		printf("Command Line Argument Number %d = %s\n", (i+1),argv[i]);
	}
	printf("\n\n");
	
	printf("First 20 Enviornmental variable Passed To This Program Are : \n\n");
	for(i=0;i<20;i++)
	{
		printf("Command Line Argument Number %d = %s\n",(i+1), envp[i]);
	}
	printf("\n\n");
	return(0);
}