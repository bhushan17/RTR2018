/*###########################################
##
##This Program Add all command line argument given 
## in integer form only and output the sum
##
##Due to use of atoi(), all command line argument of 
##types other than 'int' are ignored
###############################################*/

#include<stdio.h>
#include<ctype.h> //ctype contains declaration of atoi()

int main(int argc, char *argv[], char *envp[])
{
	//variable declaration
	int i, num, sum =0;
	
	//code
	
	printf("\n\n");
	printf("Sum of All Integer Command Line Arguments Is : \n\n");
	
	//loop start from i = 1 because, i=0 will result in argv[i] = argv[0]
	//which is the name of the program itself i.e : 'CommandLineArgumentsApplication.exe'
	for(i=1;i<argc;i++)
	{
		num = atoi(argv[i]);
		sum = sum + num;
	}
	
	printf("Sum = %d\n\n", sum);
	
	return(0);
}