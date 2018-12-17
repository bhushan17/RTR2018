/*##################################################
## Function Defination 'change_count_by_one'
###################################################*/

#include "file_02.h"

// ****************** Function defination ********************************//

extern int count;

void change_count_by_two(void)
{
	count = count + 2;
	printf("Value of count in 'change_count_by_two' function = %d\n", count);
}
