#include <cstring>
#include <stdlib.h>
#include <math.h>
//
#include "utilities.h"


#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Public functions
//-------------------------------------------------------------------
#endif

//	The C version was developed first and the C++ version, years later, simply calls
//	the C version.
char* produceOutFilePath(const char* inputImagePath, const char* suffix, 
						 const char* ext, const char* outFolderPath)
{
	//	Produce the std::string suffix + extension (e.g. " [gray].tga")		
	//--------------------------------------------------------------
	//                                                    3 for " ", ".", and '\0'
 	char* suffixPlusExt = (char*) calloc(strlen(suffix) + strlen(ext) + 3, 1);
	strcpy(suffixPlusExt, suffix);
	strcat(suffixPlusExt, ".");
	strcat(suffixPlusExt, ext);

	// Produce the name of the output file
	//-------------------------------------
	//	First, find the start of the input file's name.  Start from the end
	//	and move left until we hit the first slash or the left end of the std::string.
	unsigned long index = strlen(inputImagePath) - 5;
	while ((index>=1) && (inputImagePath[index-1] != '/'))
		index--;
	
	//	Produce the name of the input file minus extension
	char* inputFileRootName = (char*) malloc(strlen(inputImagePath+index) +1);
	strcpy(inputFileRootName, inputImagePath+index);
	//	chop off the extension by replacing the dot by '\0'
	inputFileRootName[strlen(inputFileRootName)-4] = '\0';

	char* outFilePath = (char*) malloc(strlen(outFolderPath) +
										strlen(inputFileRootName) + strlen(suffixPlusExt) + 2);
	strcpy(outFilePath, outFolderPath);
	//	If outFolderPath didn't end with a slash, add it
	if (outFolderPath[strlen(outFolderPath)-1] != '/')
		strcat(outFilePath, "/");

	//	Append root name to output path, add suffix
	strcat(outFilePath, inputFileRootName);
	strcat(outFilePath, suffixPlusExt);
	
	//	free heap-allocated data we don't need anymore
	free(inputFileRootName);
	free(suffixPlusExt);
	
	return outFilePath;
}

std::string produceOutFilePath(const std::string& inputImagePath, const std::string& suffix, 
						  	   const std::string& ext, const std::string& outFolderPath)
{
	char* filePath = produceOutFilePath(inputImagePath.c_str(), suffix.c_str(), 
						  				ext.c_str(), outFolderPath.c_str());
	std::string filePathStr(filePath);
	delete []filePath;
	
	return filePathStr;
}
