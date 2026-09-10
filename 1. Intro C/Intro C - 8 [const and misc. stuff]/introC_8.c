/*--------------------------------------------------------------+
|		A simple C program to demonstrate some basic elements	|
|	of the language to Java programmers.						|
|																|
|	Jean-Yves Hervé, 3DG URI									|
|	2024-07-10													|
+--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>	//	for bool type in C99 and newer

#if 0
#pragma mark -
//--------------------------------------------------------------------------
#pragma mark Function prototypes
//--------------------------------------------------------------------------
#endif

float sumOfTerms(int n, float* y);


#if 0
#pragma mark -
//--------------------------------------------------------------------------
#pragma mark Function implementations
//--------------------------------------------------------------------------
#endif

float sumOfTerms(int n, float* y)
{
	float sum = 0.f;
	for (int k=0; k<n; k++)
		sum += y[k];
	
	return sum;
}


//--------------------------------------------------------------------
//	Part 1:  Demo about the const modifier in C
//--------------------------------------------------------------------
void part1()
{
	typedef struct MyType 
	{
		int a;
		double b;
		
	} MyType;
	
	//	The const modifier was introduced in C89.  You may (should) be accustomed to 
	//	const in C++.  There is, however a difference between the C and C++ versions.

	//	Reminder:
	//	In C++, the const modifier applies to the variable
	//		int	x = 4;			//	x is a regular variable of type int
	//		x = 6;				//	and therefore I can change its value
	//		x++;
	//
	//		const int y = 4;	//	y is a const variable of type int (value can't be changed)
	//		y = 6; ---->  not allowed
	//		y++;   ---->  not allowed

	//	change to 0 or 1 accordingly
	#define ENABLE_INVALID_CODE_P1		0
	#define ENABLE_INVALID_CODE_P2		0
	#define ENABLE_BAD_CODE_P3			0
	
	//	In C, the const modifier applies to the type
	
	//	x is a variable of type "const int", an int that cannot be modified
	const int x = 4;	
	#if ENABLE_INVALID_CODE_P1
		x = 6;
		x++;
	#endif
	
	//	Note that the const modifier can come before or after the type it applies to.
	MyType const z = {12, 4.5};
	#if ENABLE_INVALID_CODE_P1
		z.a = 6;
		z.b++;
	#endif
		
	//	To shut up "variable not used" warning.  These statements don't modify the
	//	value of x and y;  they just force an evaluation (therefore a "use" of the variable)
	(void) x;
	(void) z;	

	//	A note about const and pointers
	//-----------------------------------
	
	//	The simple rule of thumb (which also applies in C++) is:
	//		If const appears on the left side of the * then what is being pointed to is constant
	//		If const appears on the right side of the * then the pointer is constant
	
	int n = 12;
	
	//	a is a pointer to const int data.  So, a can be modified, but not what it points to
	const int *a = &n;
	#if ENABLE_INVALID_CODE_P2
		*a = 6;		//	not valid:  what a points to cannpot be modified
		a = NULL;	//	valid:  a can be modified
	#endif
	
	//	b is a const pointer.  What it points to can be modified, but not not the pointer itself
	int * const b = &n;
	#if ENABLE_INVALID_CODE_P2
		*b = 6;		//	valid:  what b points to can be modified
		b = NULL;	//	invalid:  b cannot be modified
	#endif
	
	//	Be careful that C is *very* casual and messy about implicit typecasts
	//	Unexpected behavior can result
	//------------------------------------------------------------------------
	int m = 20;
	const int p = 20;
	printf("Before:  m = %d\n", m);
	printf("Before:  p = %d\n", p);

	#if ENABLE_BAD_CODE_P3

		//	Nothing wrong here.
		int* c = &m;

		//	This only produces a warning.  This is interpreted as an implicit typecast
		//	that casts away the constness of the data pointed to.
		int* d = &p;

		printf("Pointers:  &m = %p     c = %p\n", &m, c);
		printf("Pointers:  &p = %p     d = %p\n", &p, d);
		
		//	Nothing wrong here 
		*c = 10;
		
		//	Nothing reported here.  d is a regular pointer to (non-const) int data
		*d = 10;

		//	but check the values
		printf("Now:  m = %d and *c = %d\n", m, (*c));
		printf("Now:  p = %d and *d = %d\n", p, (*d));
		
	#endif

	//	To shut up "variable not used" warning.  These statements don't modify the
	//	value of x and y;  they just force an evaluation (therefore a "use" of the variable)
	(void) x;
	(void) z;	
	(void) a;
	(void) b;
	
}

//--------------------------------------------------------------------
//	Part 2:  Function name
//--------------------------------------------------------------------
void part2()
{
}



int main(int argc, const char * argv[]) {

	//--------------------------------------------------------------------
	//	Part 1:  Simple function with no argument and no return type
	//--------------------------------------------------------------------
	part1();
	
	//--------------------------------------------------------------------
	//	Part 2:  Function with arguments and return type
	//--------------------------------------------------------------------
	part2();
	
    return 0;
}


