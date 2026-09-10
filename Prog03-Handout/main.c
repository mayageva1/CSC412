//
//  main.c
//  Handout code for Prog 02, CSC412 - Spring 2023
//
//	This small program shows how to use the image I/O library
//	to read and write images in the uncompressed TGA file format
//	and perform some simple IP operations
//
//  Created by Jean-Yves Hervé on 2018-09-18, rev. 2025-09-26
//

#include <stdio.h>
#include <stdlib.h>	//	for malloc & calloc
#include <string.h>	//	for memcpy
//
#include "ImageIO.h"
#include "channel.h"
#include "flipV.h"

// //----------------------
// //	Function prototypes
// //----------------------
// void mirrorImage(RasterImage* image1, RasterImage* image2);	
// void killGreenChannel(RasterImage* image);

//----------------------
//	Hard-coded paths
//----------------------
//#define INPUT_PATH		"./Images/bottles.tga"
#define INPUT_PATH		"./Images/Tyler.tga"
#define OUTPUT_PATH1	"./Output/out1.tga"
#define OUTPUT_PATH2	"./Output/out2.tga"
#define OUTPUT_PATH3	"./Output/out3.tga"


int main(int argc, char** argv)
{
	//	Please note that in all my function calls, phen passing arguments, I don't
	//	send a copy of a RasterImage object, but a pointer to the object.
	//	On the other hand, when a function produces a new RasterImage, I return
	//	the RasterImage rather than a pointer to it.

	//-----------------------------------------------------------------------------
	//	Part 1
	//	I read my image using a hard-coded path.  Obviously, you would want to
	//	get the path from the list of arguments.
	//-----------------------------------------------------------------------------
	RasterImage image1 = readImage(INPUT_PATH);
	
	//	We only work with color images
	if (image1.type != RGBA32_RASTER)
	{
		printf("The file %s does not contain a color image.\n", INPUT_PATH);
		exit(10);
	}
	
	//-----------------------------------------------------------------------------
	//	Part 2
	//	I compute the vertical mirror of my image by calling the flipV function
	//		- prototype defined in includes/flipV.h, 
	//		- function implemented in src/operations.c
	//-----------------------------------------------------------------------------
	RasterImage image2 = flipV(&image1);

	//-----------------------------------------------------------------------------
	//	Part 3
	//	I eliminate the green component in image1 by calling the killGreen function
	//		- prototype defined in includes/channel.h, 
	//		- function implemented in src/operations.c
	//-----------------------------------------------------------------------------
	killGreenChannel(&image1);

	//-----------------------------------------------------------------------------
	//	Part 4
	//	I create a blank (well, really, all black) image with the same dimensions as
	//	image1 and I fill it with some computed colors
	//-----------------------------------------------------------------------------
	RasterImage image3 = newImage(image1.width, image1.height, RGBA32_RASTER);

	//	My little color generation code can only work in range 0-255
    unsigned int m = image1.height < 256 ? image1.height : 255;
    unsigned int n = image1.width < 256 ? image1.width : 255;

	//	access directly the image's raster
	unsigned char* raster = (unsigned char*)(image3.raster);

	for (unsigned int i=0; i<m; i++)
	{
		//	position my index at the beginning of the i-th row (4 bytes per pixel)
		unsigned long idx = 4 * image1.width * i;

		for (unsigned int j=0; j<n; j++)
		{
			raster[idx++] = (unsigned char) i;
			raster[idx++] = (unsigned char) (255-j);
			raster[idx++] = (unsigned char) 0;
			raster[idx++] = (unsigned char) 255;	//	alpha channel
		}
	}
			
	//-----------------------------------------------------------------------------
	//	Part 5
	//	Output the images
	//-----------------------------------------------------------------------------
	writeImage(&image1, OUTPUT_PATH1);
	writeImage(&image2, OUTPUT_PATH2);
	writeImage(&image3, OUTPUT_PATH3);

	//-----------------------------------------------------------------------------
	//	Part 6
	//	Here I am in the main function, so all memory allocated would get freed when 
	//	I quit, but, just for the purpose of keeping good habits, I delete my rasters.
	//-----------------------------------------------------------------------------
	free(image1.raster);
	free(image2.raster);
	free(image3.raster);
	
	return 0;
}


