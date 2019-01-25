/******************************************
**User Defined Function : Method of Defination 4
**Valid (int) return value, Valid parameter (int, int)
**
**
********************************************/

#include<stdio.h>

int main(int argc, char *argv[], char *envp[])
{
	//function declaration
	int MyAddition(int, int);
	
	//variable declaration : local variable to main()
	int a,b, result;
	
	//code
	printf("\n\n");
	printf("Enter Integer value For 'A' : ");
	scanf("%d",&a);
	
	printf("\n\n");
	printf("Enter Integer value For 'B' : ");
	scanf("%d",&b);
	
	result = MyAddition(a,b);//Function call
	printf("\n\n");
	printf("Sum of %d And %d = %d\n\n",a, b, result);
	
	return(0);
}


int MyAddition(int a, int b)
{
	//variable declaration : local variable to MyAddition()
	int sum;
	
	sum = a+b;
	return(sum);
	
}
