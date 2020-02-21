
#include"vectore.h"

(float**) create_paticleVectore(void)
{
	float** localptr = NULL;
    localptr = (float**)malloc(sizeof(float*));
	memset(localptr,0,sizeof(float));
	sizeOfPosArray++;
	return localptr;
	
}

Push_back_paticle(float** pos_vec, float* coordinate)
{
	pos_vec = (float**)realloc(pos_vec, (sizeOfPosArray+1)*sizeof(float*));
	
	(*pos_vec)[sizeOfPosArray -1][0] = coordinate[0];
	(*pos_vec)[sizeOfPosArray -1][1] = coordinate[1];
	(*pos_vec)[sizeOfPosArray -1][2] = coordinate[2];
	
	sizeOfPosArray++;
	
}

