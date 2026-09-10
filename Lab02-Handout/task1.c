//  Lab02 Task1
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>	//	for isdigit

int main(int argc, char* argv[])
{
	//	In labs, we haven't got the time to do all the correct things about data
	//	validation.  So here I simply assume argc == 2
	//	(and we'll just seg-fault if that's not the case)
	//	OK for labs, absolutely not for programming assignments.
	//any pointer is 8bytes
	char* inStr = argv[1];

	printf("%s\n", inStr);

//	Just to show that we can "shorten" a string by writing a 0 in the array of char
    //inStr[5] = 0;
   	//printf("%s\n", inStr);
		
	//	I could store my length in an int, but then I would have to do a casting
	//	to suppress the warning.  Please keep in mind that size_t is an unsigned
	//	integral type (typically unsigned long), and that you will get warnings
	//	if you assign to or compare with variables of signed int type.

	//size of input. size_t is a long which uses 8bytes of memory.
	size_t strLength = strlen(inStr);

	//declare array to hold the categories.
	char* digits = (char*)malloc(sizeof(char)*strLength);
	for (size_t k=0; k<strLength; k++)
	{	
		if (isdigit(inStr[k]))
		//	same thing
//		if (inStr[k]>='0' && inStr[k]<='9')
		{	
		}
		else if (isalpha(inStr[k])){

		}
		else{

		}
	}
		
	return 0;
}

//compile: gcc -Wall -std=c11 task1.c -o task1-test
//./task1-test