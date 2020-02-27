
#include "Stack.h"

#define MAX 10
#define MIN 0

mat4 stackArry[MAX];
int topindex = 0;

int BRKStackPush(mat4 matrix)
{
	if (topindex == MAX)
		return STACKFULL;

	stackArry[topindex] = matrix;
	topindex++;
	return SUCCESS;
}


int BRKPopMatrix(mat4* matrix)
{
	if (topindex == MIN)
		return STACKEMPTY;

	
	*matrix = stackArry[--topindex];
	//topindex--;
	return SUCCESS;
}