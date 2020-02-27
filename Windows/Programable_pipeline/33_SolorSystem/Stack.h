
#include "vmath.h"

using namespace vmath;


enum {
	SUCCESS = 0,
	FAILED,
	ERR,
	STACKFULL,
	STACKEMPTY
};

int BRKStackPush(mat4 matrix);
int BRKPopMatrix(mat4* matrix);
