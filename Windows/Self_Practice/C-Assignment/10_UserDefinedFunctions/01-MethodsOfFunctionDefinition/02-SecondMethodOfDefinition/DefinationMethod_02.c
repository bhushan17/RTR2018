/******************************************
**User Defined Function : Method of Defination 2
** Valid (int) return value, No Parameters
**
**
********************************************/

#include<stdio.h>

int main(int argc, char *argv[], char *envp[])
{
	//function declaration
	int MyAddition(void);
	
	//variable declaration : local variable to main()
	int result;
	
	//code
	result = MyAddition();//Function call
	
	printf("\n\n");
	printf("Sum = %d\n\n", result);
	return(0);
}


int MyAddition(void)
{
	int a,b,sum;
	
	printf("\n\n");
	printf("Enter Integer value For 'A' : ");
	scanf("%d",&a);
	
	printf("\n\n");
	printf("Enter Integer value For 'B' : ");
	scanf("%d",&b);
	
	sum = a+b;
	
	return(sum);
}
