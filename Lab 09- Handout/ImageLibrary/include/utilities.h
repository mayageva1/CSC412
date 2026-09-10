#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>

/**	Produces a complete path to the output image file.
 *	If the input file path was ../../Images/cells.tga 
 *  the suffix to add is [gray],
 *	and the output folder path is ../Output (with or without final slash),
 *	then the output file path will be ../Output/cells [gray].tga
 *
 *	@param inputImagePath	path to the input image
 *  @param suffix			the suffix to add to the base file name
 *  @param ext				the file extension (decides of image file type)
 *	@param outFolderPath	path to the output folder
 *	@return	complete path to the desired output file.
 */
char* produceOutFilePath(const char* inputImagePath, const char* suffix, 
						 const char* ext, const char* outFolderPath);

/**	Produces a complete path to the output image file.
 *	If the input file path was ../../Images/cells.tga 
 *  the suffix to add is [gray],
 *	and the output folder path is ../Output (with or without final slash),
 *	then the output file path will be ../Output/cells [gray].tga
 *
 *	@param inputImagePath	path to the input image
 *  @param suffix			the suffix to add to the base file name
 *  @param ext				the file extension (decides of image file type)
 *	@param outFolderPath	path to the output folder
 *	@return	complete path to the desired output file.
 */
std::string produceOutFilePath(const std::string& inputImagePath, const std::string& suffix, 
						  	   const std::string& ext, const std::string& outFolderPath);

#endif // UTILITIES_H
