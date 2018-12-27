/*##################################################
## Function Defination 'change_count_by_one'
###################################################*/

#include "file_01.h"

// ****************** Function defination ********************************//

extern int count;

void change_count_by_one(void)
{
	count = count + 1;
	printf("Value of count in 'change_count_by_one' function = %d\n", count);
}
