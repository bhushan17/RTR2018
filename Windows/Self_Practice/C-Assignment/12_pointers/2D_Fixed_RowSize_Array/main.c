/*##################################################
## 
## Print value and address of 2D fixed array.
##
###################################################*/

//****************** Includes header files **************************//
#include<stdio.h>
#include<stdlib.h>

//****************** Declare Global variables **************************//


//****************** Declare function defination **************************//




// ****************** Function defination ********************************//


int main(void)
{
	int *a[5];
	int i,j,colum;
	
	printf("\t\n Enter the size of colum  = ");
	scanf("%d",&colum);
	
	
	for(i=0;i<5;i++)
		a[i]=(int*)malloc(colum * sizeof(int));
	
	
	for(i=0;i<5;i++)
	{
		for(j=0;j<colum;j++)
		{
			printf("\t \nEnter the Value of a[%d][%d] =  ",i,j);
			scanf("%d",&a[i][j]);
			
		}
	}
	
	for(i=0;i<5;i++)
	{
		for(j=0;j<colum;j++)
		{
			printf("\t Value of a[%d][%d] = %d \n", i,j,a[i][j]);
			printf("\t Address of a[%d][%d] = %p \n", i,j, &a[i][j]);
		}
	}
		
	return (0);
}