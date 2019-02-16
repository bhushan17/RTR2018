/*##################################################
## 
## Print value and address of 2D fixed array.
##
###################################################*/

//****************** Includes header files **************************//
#include<stdio.h>

//****************** Declare Global variables **************************//


//****************** Declare function defination **************************//




// ****************** Function defination ********************************//


int main(void)
{
	int a[5][3]={{4,9,1},{2,3,5},{7,8,6},{13,14,11},{12,15,10}};
	int i,j;
	
	for(i=0;i<5;i++)
	{
		for(j=0;j<3;j++)
		{
			printf("\t Value of a[%d][%d] = %d \n", i,j,a[i][j]);
			printf("\t Address of a[%d][%d] = %p \n", i,j, &a[i][j]);
		}
	}
		
	return (0);
}