/*##################################################
## Problem Statement : Roy wants to change his profile picture on Facebook. Now Facebook has some restriction over the dimension of picture that we can upload.
##                     Minimum dimension of the picture can be L x L, where L is the length of the side of square. 
##
## Constraints:
## 1 <= L,W,H <= 10000
## 1 <= N <= 1000
###################################################*/

//****************** Includes header files **************************//
#include<stdio.h>
#include"Stdtype.h"


//****************** Declare Global variables **************************//

//****************** Function Declaration ******************************// 


//******* Entry Function ****************//
int main()
{
	int length=ZERO, noOfPhotho=ZERO, photoNumber=ZERO;
	int *width;
	int *height;
	
	
	do
	{
		printf("Enter the length dimention of standered photo between %d and %d.\n", MIN_PHOTO_SIZE, MAX_PHOTO_SIZE);
		scanf("%d", &length);
	} while ((length < MIN_PHOTO_SIZE) || (length > MAX_PHOTO_SIZE));
	
	do
	{
		printf("Enter the number of photo you want to check between %d and %d.\n", MIN_NUM_PHOTO, MAX_NUM_PHOTO);
		scanf("%d", &noOfPhotho);
	} while ((noOfPhotho < MIN_NUM_PHOTO) || (noOfPhotho > MAX_NUM_PHOTO));

	width = (int*)malloc(noOfPhotho * sizeof(int));
	height = (int*)malloc(noOfPhotho * sizeof(int));
	
	for (photoNumber=ONE; photoNumber <= noOfPhotho; photoNumber++)
	{
		do {
			printf("Enter the width and height of %d photo in the range of %d and %d \n", photoNumber, MIN_PHOTO_SIZE, MAX_PHOTO_SIZE);
			scanf("%d %d", &(width[(photoNumber-ONE)]), &(height[(photoNumber-ONE)]));
		} while ((width[photoNumber-1] < MIN_PHOTO_SIZE) || (width[photoNumber-1] > MAX_PHOTO_SIZE) || (height[photoNumber-1] < MIN_PHOTO_SIZE) || (height[photoNumber-1] > MAX_PHOTO_SIZE));
	}

	
	for (photoNumber = ONE; photoNumber <= noOfPhotho; photoNumber++)
	{
		if ((width[(photoNumber-1)] < length) || (height[(photoNumber-1)] < length))
		{
			printf("UPLOAD ANOTHER.\n");
		}
		else if ((width[photoNumber - 1] == length) && (height[photoNumber - 1] == length))
		{
			printf("ACCEPTED.\n");
		}
		else
		{
			printf("CROP IT.\n");
		}
			
	}




	getch();
	return (0);
}






 