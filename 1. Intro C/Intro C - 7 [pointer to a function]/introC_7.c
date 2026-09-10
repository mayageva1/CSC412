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

void part1();
void part2();
void part3();

// 1. When we write this, this is commonly interpreted as meaning that
//	"simpleFunc is a function that takes no argument and returns nothing." 
// Well, really, here, we do define a a function that takes no argument and 
// returns nothing, and the value of simpleFunc is a pointer to that function
void simpleFunc();

// 2. In the same order of idea, the value of sumOfTerms (and of course sumOfSquaredTerms)
// is a pointer to a function that takes an int and a pointer to float (an array of floats)
// as arguments and returns a float value.
float sumOfTerms(int n, float* y);
float sumOfSquaredTerms(int n, float* y);


#if 0
#pragma mark -
//--------------------------------------------------------------------------
#pragma mark Function implementations
//--------------------------------------------------------------------------
#endif

void simpleFunc()
{
	static int numCalls = 0;
	
	numCalls++;
	printf("In simpleFunc, call number %d\n", numCalls);
}

float sumOfTerms(int n, float* y)
{
	float sum = 0.f;
	for (int k=0; k<n; k++)
		sum += y[k];
	
	return sum;
}


float sumOfSquaredTerms(int n, float* y)
{
	float sum = 0.f;
	for (int k=0; k<n; k++)
		//	no, I don't use powf
		sum += y[k]*y[k];
	
	return sum;
}

//--------------------------------------------------------------------
//	Part 1:  Simple function with no argument and no return type
//--------------------------------------------------------------------
void part1()
{
	printf("\nPart 1\n------\n");
	printf("Point 1:  First direct call to simpleFunc.\n");
	simpleFunc();
	
	//	Let's decode this:  I am saying that what someFunction points to (the value of *someFunction)
	//  	- is a function with no arguments --> ()
	//  	- returns nothing  --> void
	void (*someFunction)() = simpleFunc;
	
	//	and so I can call this function
	printf("Point 2:  call to simpleFunc as sumeFunction.\n");
	someFunction();
	
	printf("Point 3:  Second direct call to simpleFunc.\n");
	simpleFunc();
}

//--------------------------------------------------------------------
//	Part 2:  Function with arguments and return type
//--------------------------------------------------------------------
void part2()
{
	printf("\nPart 2\n------\n");
	
	//	define the array and determine its size
	float a[] = {10.f, -1.4f, 3.f, 8.f, 4.5f, 6.2f};
	int sizeA = (int) (sizeof(a)/sizeof(a[0]));
	
	printf("array a stores %d elements:  {", sizeA);
	for (int k=0; k<sizeA-1; k++)
		printf("%f, ", a[k]);
	printf("%f}\n", a[sizeA-1]);
	
	printf("For reference, sum of elements of a is %f and sum of squared terms is %f\n\n",
			sumOfTerms(sizeA, a), sumOfSquaredTerms(sizeA, a));

	// 	What funcPt points to is a function that takes as arguments an int and a pointer 
	//	to float , and returns a float value
	float (*funcPt)(int, float*) = NULL;

	bool keepAsking = true;
	
	while (keepAsking)
	{
		printf("Which function do you want to use?\n");
		printf("\t(1)  sum of terms, (2)  sum of squares of terms, (0)  end loop:  ");
		int choice;
		scanf("%d", &choice);
		switch (choice)
		{
			case 0:
				funcPt = NULL;
				keepAsking = false;
				break;
			case 1:
				printf("\tYou selected function 1 (sum of terms)\n");
				// now funcPt points to 
				funcPt = sumOfTerms;
				break;
			case 2:
				printf("\tYou selected function 2 (sum of squared terms)\n");
				funcPt = sumOfSquaredTerms;
				break;
			default:
				printf("\t\tType either 1 or 2, you knucklehead!\n");
				break;
		}
		if (funcPt != NULL)
		{
			float val = funcPt(sizeA, a);
			printf("value computed = %f\n", val);
		}	
	}
}


//--------------------------------------------------------------------
//	Part 3:  Function passed as an argument
//--------------------------------------------------------------------

//	This function receives as arguments a C string and a (pointer to a) function that must 
//	be called to process local data
void processData(char* funcName, float (*processingFunction)(int, float*))
{
	//	define the array and determine its size
	float a[] = {-8.5f, -4.75f, 13.f, 7.f, 4.5f};
	int sizeA = (int) (sizeof(a)/sizeof(a[0]));

	printf("In function processData, local array a stores %d elements:  {", sizeA);
	for (int k=0; k<sizeA-1; k++)
		printf("%f, ", a[k]);
	printf("%f}\n", a[sizeA-1]);

	printf("processData has been called with processing function %s and the computed value is: %f\n",
			funcName, processingFunction(sizeA, a));
			
}

void part3()
{
	printf("\nPart 3\n------\n");
	
	processData("sumOfTerms", sumOfTerms);
	processData("sumOfSquaredTerms", sumOfSquaredTerms);
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
	
	//--------------------------------------------------------------------
	//	Part 3:  Function passed as an argument
	//--------------------------------------------------------------------
	part3();
		

    return 0;
}


