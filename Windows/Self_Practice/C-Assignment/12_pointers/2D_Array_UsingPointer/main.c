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
	int **ptr;
	int i,j,row,colum;
	
	printf("\t\nEnter the size of row  = ");
	scanf("%d",&row);
	
	printf("\t\nEnter the size of colum  = ");
	scanf("%d",&colum);
	
	ptr=(int**)malloc(row * sizeof(int*));
	
	for(i=0;i<row;i++)
		ptr[i]=(int*)malloc(colum * sizeof(int));
	
	
	for(i=0;i<row;i++)
	{
		for(j=0;j<colum;j++)
		{
			printf("\t\n Enter the Value of a[%d][%d] = ",i,j);
			scanf("%d",&ptr[i][j]);
			
		}
	}
	
	for(i=0;i<5;i++)
	{
		printf ("\t\t\nAddress of Row = %p",ptr+i);
		for(j=0;j<colum;j++)
		{
			printf("\t\n Value of a[%d][%d] = %d ", i,j,ptr[i][j]);
			printf("\t\n Address of a[%d][%d] = %p ", i,j, &ptr[i][j]);
		}
		
	}
		
	return (0);
}