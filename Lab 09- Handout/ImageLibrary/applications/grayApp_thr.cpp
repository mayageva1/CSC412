/*----------------------------------------------------------------------------------+
|	gray	produced a copy of the input image that has been converted to gray		|
|																					|
|	Usage:																			|
|			./gray <image file path>  <output folder path>							|
|	If the image is named bottles.tga, and the rotation selected was -l, then 		|
|	the image produced is 															|
|		bottles [gray].tga																|
|																					|
|	Jean-Yves Hervé, 2020-10-29														|
+----------------------------------------------------------------------------------*/
#include <iostream>
#include <cstdlib>
#include <cstring>
//
#include "ImageIO.h"
#include "utilities.h"
#include "gray.h"

using namespace std;

#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Custom data types and global variables
//-------------------------------------------------------------------
#endif

/**	An enum type for all the errors that this program specifically handles
 */
using ErrorCode = enum
{
	NO_ERROR = 0,
	//
	//	1x codes:	file-related	--> not used in this program
//	FILE_NOT_FOUND = 10,
//	CANNOT_OPEN_FILE = 11,
//	WRONG_FILE_TYPE = 12,
//	CANNOT_WRITE_FILE = 13,
	//
	//	4x codes:	command line argument errors
	WRONG_NUMBER_OF_ARGUMENTS = 30,
	
};



#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Function implementations
//-------------------------------------------------------------------
#endif

//--------------------------------------------------------------
//	Main function, expecting as arguments:
//		inputImagePath outFolderPath
//	It returns an error code (0 for no error)
//--------------------------------------------------------------
int main(int argc, const char* argv[])
{
	//	We need 2 arguments: filePath outputPath
	//	We need 3 arguments: filePath outputPath number of threads
	if (argc != 4)
	{
		cout << "Proper usage: " << argv[0] << " inputImagePath outFolderPath num_threads" << endl;
		return WRONG_NUMBER_OF_ARGUMENTS;
	}
	
	//	Just to look prettier in the code, I give meaningful names to my arguments
	const char* inputImagePath = argv[1];
	const char* outFolderPath = argv[2];
	unsigned int numThreads;
	sscanf(argv[3], "%ud", &numThreads);

	//	Read the image
	RasterImage image = readImage(inputImagePath);

	//	Perform the conversion to gray
	RasterImage grayImage = toGray_thr(image, numThreads);

	// Produce the path to the output file
	const char* outFilePath = produceOutFilePath(inputImagePath, " [gray thr]", "tga", outFolderPath);

	//	Write out the gray image
	int err = writeImage(grayImage, outFilePath);

	//	Cleanup allocations.  Again, this is not really needed, since the full
	//	partition will get cleared when the process terminates, but I like to
	//	keep the good habit of freeing memory that I don't need anymore, and,
	//	if I crash, it's a sign that something went wrong earlier and I may
	//	have produced junk
	freeImage(image);
	freeImage(grayImage);
	free(const_cast<char*>(outFilePath));

	return err;
}
