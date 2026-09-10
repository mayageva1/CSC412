/*--------------------------------------------------------------+
|		A simple C program to demonstrate some basic elements	|
|	of the language to Java programmers.						|
|	Subset of the code sample posted earlier					|
|																|
|	Jean-Yves Hervé, 3DG URI									|
|	2019-09-03, rev. 2020-10-06, 2021-10-11						|
+--------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------
//	Function prototypes
//-------------------------

void part1(void);
void part2(void);

//-------------------------
//	Some constants
//-------------------------
#define INPUT_FILE_PATH_1	"./IO_Data/input1.dat"
#define INPUT_FILE_PATH_2	"./IO_Data/input2.dat"
#define INPUT_FILE_PATH_3	"./IO_Data/input3.dat"
#define INPUT_FILE_PATH_4	"./IO_Data/input4.dat"
#define INPUT_FILE_PATH_5	"./IO_Data/input5.dat"
#define OUTPUT_FILE_PATH_A	"./Output/outA.txt"
#define OUTPUT_FILE_PATH_B	"./Output/outB.txt"

//-----------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------


int main(int argc, char* argv[])
{
	part1();
	
	part2();
	
	return 0;
}

//	Part 1: Basic file I/O
void part1(void)
{
	printf("\n\n-------------------\nPart 1\n-------------------\n");
	
	//----------------------------------
	//	File input: ASCII (text) file
	//----------------------------------
	FILE *inFile = fopen(INPUT_FILE_PATH_1, "r");
	
	//	make sure that the file was properly open.  It may not exist (bad path), or
	//	it may be busy or not accessible (access rights)
	if (inFile == NULL)
	{
		//	do something (cleanup)
		
		//	For this small sample, I simply print an error message and
		//	exit with some error code
		printf("Cannot open file \"%s\"\n", INPUT_FILE_PATH_1);
		exit(11);
	}
	
	//	I read 2 int, a float, and a string (no space) from the file
	//	Note that I need to know the maximum size of my string (+1 for the terminating 0)
	int a, b;
	float x;
	char s[81];
	
	//	We specify the source, the format of the data, and the location were we want
	//	to read the data into.
	//	The C string s is a pointer to the beginning of the string, so we should not
	//	pass &s
	int nRead = fscanf(inFile, "%d%d%f%s", &a, &b, &x, s);
	
	//	fscanf returns the number of elements that were successfully read.  So it's a 
	//	good idea to check that number
	if (nRead != 4)
	{
		printf("Something went wrong while ready the data. Only %d elements were read\n", nRead);
		exit(12);
	}
	printf("I read: a=%d, b=%d, x=%f, s=\"%s\"\n", a, b, x, s);
	
	//	don't forget to close the file when you're done
	fclose(inFile);
	
	//	Note that in the format string of the fscanf statement, "blank" characters (space
	//	and tab) are skipped.  So, I could have written 
	//			fscanf(inFile, "%d %d %f %s", &a, &b, &x, s);
	//	or even
	//			fscanf(inFile, "%d  %d\t%f \t %s", &a, &b, &x, s);
	//	It's  a different story when the format uses non-blank characters, as we shall
	//	see in Part 3.
	
	//----------------------------------
	//	File output: ASCII (text) file
	//----------------------------------
	FILE* outFile = fopen(OUTPUT_FILE_PATH_A, "w");

	//	Again, make sure that the file was properly open.  Here, the first time you
	//	run the program, it will fail because the directory Output doesn't exist.
	//	Create that directory (mkdir Output) and run again.
	if (outFile == NULL)
	{
		//	do something (cleanup)
		
		//	For this small sample, I simply print an error message and
		//	exit with some error code
		printf("Cannot open file \"%s\" for writing\n", OUTPUT_FILE_PATH_A);
		exit(13);
	}

	//	I write  back to the file the data in the same form as in the input file.
	//	Note that here I need to include the separating spaces in the format string.
	//	Also note that I don't pass a pointer to a, b, x but the *values* of these variables.
	fprintf(outFile, "%d %d %f %s\n", a, b, x, s);
	
	//	Of course, since fprintf works exactly like printf in terms of formatting, I could
	//	also produce an output file more convenient for human consumption.
	fprintf(outFile, "a=%d, b=%d, x=%6.2f, s=%s\n", a, b, x, s);
	
	//	again, don't forget to close the file
	fclose(outFile);
	
	printf("Output of ASCII file completed.\n");	
}

//	Part 2: Read data into an array
void part2(void)
{
	printf("\n\n-------------------\nPart 2\n-------------------\n");
	
	//----------------------------------
	//	File input: ASCII (text) file
	//----------------------------------
	FILE *inFile = fopen(INPUT_FILE_PATH_3, "r");
	if (inFile == NULL)
	{
		//	do something (cleanup)
		
		//	For this small sample, I simply print an error message and
		//	exit with some error code
		printf("Cannot open file \"%s\"\n", INPUT_FILE_PATH_3);
		exit(21);
	}

	//	The file stores on a first line the number of rows and columns of the array
	unsigned int numRows, numCols;
	int nRead = fscanf(inFile, "%u%u", &numRows, &numCols);
	if (nRead != 2)
	{
		printf("Something went wrong while ready the data.  Only reads %d element.\n", nRead);
		exit(22);
	}
	
	//	Allocate an array for the data
	float** a = (float**) calloc(numRows, sizeof(float*));
	for (unsigned int i=0; i<numRows; i++)
		a[i] = calloc(numCols, sizeof(float));
		
	//	Now we read, row by row
	for (unsigned int i=0; i<numRows; i++)
		for (unsigned int j=0; j<numCols; j++)
			fscanf(inFile, "%f", a[i]+j);
	
	//	close the file
	fclose(inFile);
			
	//	Print out what we read
	printf("Array read from ASCII file (%d rows and %d columns:)\n", numRows, numCols);
	for (unsigned int i=0; i<numRows; i++)
	{
		printf("\t");
		for (unsigned int j=0; j<numCols; j++)
			printf("%8.3f  ", a[i][j]);
		printf("\n");
	}	
	
	//	Unless I return the array, I must free it before leaving, to avoid leaking memory
	for (unsigned int i=0; i<numRows; i++)
	{
		free(a[i]);
	}
	free(a);	
}
