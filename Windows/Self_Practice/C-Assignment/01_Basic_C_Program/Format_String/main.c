/*##################################################
## 6. Format String
##
## int number = 13;
## i)print number in Decimal value with %d
## ii)print number in octal format by %o
## iii)print number in Hexadecimal format by
## 	%x -> (Hexadecimal Letters In Lower Case)
## 	%X -> (Hexadecimal Letters In Upper Case)
##
## 	char ch = 'P';
## iv)print number in character format by %c
## 	char str[] = "Astromedicomp";
## v)print number in string format by %s
##
## long num = 30121995L;
## vi)print num in long integer format by %ld
##
## unsigned int b = 7;
## vii)print Unsigned Integer by using %u
##
## float f_num = 3012.1995f;
## viii) print f_num using
## 	a)%f
## 	b)%4.2f
## 	c)%2.5f
##
## double d_pi = 3.14159265358979323846;
##
## ix) print d_pi using
## 	a)%g (Double Precision Floating Point Number Without Exponential)
## 	b)%e (Double Precision Floating Point Number With Exponential (Lower Case))
## 	c)%E (Double Precision Floating Point Number With Exponential (Upper Case))
## 	d)%a (Double Hexadecimal Value Of 'd_pi' (Hexadecimal Letters In Lower Case))
## 	e)%A (Double Hexadecimal Value Of 'd_pi' (Hexadecimal Letters In Upper Case))
###################################################*/

//****************** Includes header files **************************//
#include<stdio.h>

//****************** Declare Global variables **************************//


//****************** Declare function defination **************************//

// ****************** Function defination ********************************//

int main(void)
{

	int num = 13;

	// i)print number in Decimal value with %d
	printf("Print number in Decimal value with = %d  \n", num);

	//ii)print number in octal format by %o  
	printf("Print number in octal format by = %o \n", num);

	//iii)print number in Hexadecimal format by 
	//    %x -> (Hexadecimal Letters In Lower Case)
	printf("Printf Hexadecimal Letters In Lower Case = %x \n", num);

	// 	% X -> (Hexadecimal Letters In Upper Case)
	printf("Printf Hexadecimal Letters In Upper Case = %X \n", num);

	char ch = 'P';

	//iv)print number in character format by %c
	printf("Print number in character format  = %c\n", ch);

	char str[] = "Astromedicomp";
	// v)print number in string format by %s
	printf("Print number in string format = %s\n", str);

	long number = 30121995L;
	// vi)print num in long integer format by %ld
	printf("Print num in long integer format = %ld\n", number);

	unsigned int unsign_int = 7;
	// vii)print Unsigned Integer by using %u
	printf("Print Unsigned Integer = %u\n", unsign_int);

	float f_num = 3012.1995f;
	// viii) print f_num using 	a) % f
	printf("Print f_num = %f\n", f_num);
	// viii) print f_num using b) % 4.2f
	printf("Print f_num = %4.2f\n", f_num);
	// viii) print f_num using c) % 2.5f
	printf("Print f_num = %2.5f\n", f_num);

	double d_pi = 3.14159265358979323846;

	//ix) print d_pi using
	//a) % g(Double Precision Floating Point Number Without Exponential)
	printf("Print Double precision floating point number without exponential = %g\n", d_pi);

	//b) % e(Double Precision Floating Point Number With Exponential(Lower Case))
	printf("Print Double Precision Floating Point Number With Exponential 'Lower Case' = %e\n", d_pi);

	//c) % E(Double Precision Floating Point Number With Exponential(Upper Case))
	printf("Print Double Precision Floating Point Number With Exponential 'Upper Case' = %E\n", d_pi);
		
	//d) % a(Double Hexadecimal Value Of 'd_pi' (Hexadecimal Letters In Lower Case))
	printf("Print Double Hexadecimal Value Of 'd_pi' 'Hexadecimal Letters In Lower Case' = %a \n", d_pi);
		
	//e) % A(Double Hexadecimal Value Of 'd_pi' (Hexadecimal Letters In Upper Case))
	printf("Print Double Hexadecimal Value Of 'd_pi' 'Hexadecimal Letters In Upper Case' = %A\n ", d_pi);





	
	return (0);
}