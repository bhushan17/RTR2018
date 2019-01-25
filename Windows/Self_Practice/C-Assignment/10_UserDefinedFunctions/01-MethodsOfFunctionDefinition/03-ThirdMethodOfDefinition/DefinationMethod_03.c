/******************************************
**User Defined Function : Method of Defination 3
**No Return Value, Valid parameter (int, int)
**
**
********************************************/

#include<stdio.h>

int main(int argc, char *argv[], char *envp[])
{
	//function declaration
	void MyAddition(int, int);
	
	//variable declaration : local variable to main()
	int a,b;
	
	//code
	printf("\n\n");
	printf("Enter Integer value For 'A' : ");
	scanf("%d",&a);
	
	printf("\n\n");
	printf("Enter Integer value For 'B' : ");
	scanf("%d",&b);
	
	MyAddition(a,b);//Function call
	
	
	return(0);
}


void MyAddition(int a, int b)
{
	int sum;
	
	sum = a+b;
	printf("\n\n");
	printf("Sum = %d\n\n", sum);
	
	
	
	
	sum = a+b;
	
	return(sum);
}
