/******************************************
**User Defined Function : Method of Defination 1
**No Return Value, No Parameters
**
**
**
**
********************************************/

#include<stdio.h>

int main(int argc, char *argv[], char *envp[])
{
	//function declaration
	void MyAddition(void);
	
	//code
	MyAddition();//Function call
	return(0);
}


void MyAddition(void)
{
	int a,b,sum;
	
	printf("\n\n");
	printf("Enter Integer value For 'A' : ");
	scanf("%d",&a);
	
	printf("\n\n");
	printf("Enter Integer value For 'B' : ");
	scanf("%d",&b);
	
	sum = a+b;
	
	printf("\n\n");
	printf("Sum of %d And %d = %d\n\n",a ,b, sum);
}
