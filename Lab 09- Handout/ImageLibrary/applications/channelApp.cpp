//
//  channel.cpp
//  Handout code for Lab 09, CSC412
//
//	Usage:	channel -r|g|b <input image path>  <output folder path>
//
//	This small program "kills" one color channel of the input image
//	The output image gets the killed channel as prefix to its name
//	e.g.   Tyler.tga ---> [r] Tyler.tga
//
//  Created by Jean-Yves Hervé on 2024-03-01
//

#include <cstdio>
#include <cstdlib>	//	for malloc & calloc
#include <cstring>	//	for memcpy
//
#include "ImageIO.h"
#include "utilities.h"
#include "channel.h"


int main(int argc, const char* argv[])
{
	if (argc != 4)
	{
		printf("Usage: channel -r|g|b <input image path>  <output folder path>\n");
		exit(1);
	}
	
	//	Get the arguments.  Check for validity and "cleans up" the folder path
	const char* channelChoiceStr = argv[1];
	const char* inputImagePath = argv[2];
	const char* outputFolderPath = argv[3];

	//	I read the image first so that I only need to test once the channel choice
	//	(pure coding laziness)
	RasterImage image = readImage(inputImagePath);
	//	We only work with color images
	if (image.type != RGBA32_RASTER)
	{
		printf("The file %s does not contain a color image.\n", inputImagePath);
		exit(10);
	}

	//-----------------------------------------------------------------------------
	//	Build output file path
	//-----------------------------------------------------------------------------
	char* outputFilePath;
	if (strcmp(channelChoiceStr, "-r")==0)
	{
		outputFilePath = produceOutFilePath(inputImagePath, " [-r]", "tga", outputFolderPath);
		killRedChannel(image);
	}
	else if (strcmp(channelChoiceStr, "-g") == 0)
	{
		outputFilePath = produceOutFilePath(inputImagePath, " [-g]", "tga", outputFolderPath);
		killGreenChannel(image);
	}
	else if (strcmp(channelChoiceStr, "-b") == 0)
	{
		outputFilePath = produceOutFilePath(inputImagePath, " [-b]", "tga", outputFolderPath);
		killBlueChannel(image);
	}
	else
	{
		printf("Invalid color channel: %s\n", channelChoiceStr);
		exit(2);
	}
				
	//-----------------------------------------------------------------------------
	//	Output the image
	//-----------------------------------------------------------------------------
	writeImage(image, outputFilePath);
	
	//	Here I am in the main function, so all memory allocated would get freed when 
	//	I quit, but, just for the purpose of keeping good habits, I delete my rasters.
	freeImage(image);
	free(outputFilePath);
	
	return 0;
}

