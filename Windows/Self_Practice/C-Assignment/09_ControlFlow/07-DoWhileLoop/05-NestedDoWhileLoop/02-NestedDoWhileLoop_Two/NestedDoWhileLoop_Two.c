#include <stdio.h>
int main(void)
{
	//variable declarations
	int outerloop, middleloop, innerloop;

	//code
	printf("\n\n");
	
	outerloop = 1;
	do
	{
		printf("outerloop = %d\n", outerloop);
		printf("--------\n\n");
		
		middleloop = 1;
		do
		{
			printf("\tmiddleloop = %d\n", middleloop);
			printf("\t--------\n\n");
			
			innerloop = 1;
			do
			{
				printf("\t\tinnerloop = %d\n", innerloop);
				innerloop++;
			}while (innerloop <= 3);
			printf("\n\n");
			middleloop++;
		}while (middleloop <= 2);
		printf("\n\n");
		outerloop++;
	}while (outerloop <= 2);
	return(0);
}
