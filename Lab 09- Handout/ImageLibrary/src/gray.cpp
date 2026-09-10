#include <algorithm>	//	for max
#include <cstring>		// for memcpy and strcmp
//
#include "gray.h"

using namespace std;

#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Prototypes of "private" functions
//-------------------------------------------------------------------
#endif

/**
 * Returns the unweighted average among ther red, green, blue channels
 * of a pixel
 * @param rgba	pointer to the start of an RGBA32 pixel
 */
unsigned char toGrayAvg_(const unsigned char* rgba);

/**
 * Returns the max value among ther red, green, blue channels of a pixel
 * @param rgba	pointer to the start of an RGBA32 pixel
 */
unsigned char toGrayMax_(const unsigned char* rgba);


#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Public functions
//-------------------------------------------------------------------
#endif


RasterImage toGray(const RasterImage& imageIn)
{
	RasterImage imageOut = newImage(imageIn.width, imageIn.height,
									GRAY_RASTER, 1);
		
	switch (imageIn.type)
	{
		//	image out is simply a copy of image in
		case GRAY_RASTER:
			memcpy(imageOut.raster, imageIn.raster,
				   imageIn.height*imageIn.bytesPerRow);
			break;
			
		case RGBA32_RASTER:
			{
				const int** rasterIn = static_cast<const int**>(imageIn.raster2D);
				unsigned char** rasterOut = static_cast<unsigned char**>(imageOut.raster2D);
				for (unsigned short i=0; i<imageIn.height; i++)
				{
					for (unsigned short j=0; j<imageIn.width; j++)
					{
						const unsigned char* rgba = reinterpret_cast<const unsigned char*>(rasterIn[i] + j);
						//	access pixel at column j on row i
						rasterOut[i][j] = toGrayMax_(reinterpret_cast<const unsigned char*>(rgba));
					}
				}
			}
			break;
		
		//	unsupported
		case FLOAT_RASTER:
		default:
			exit(100);
			break;
	}
	
	return imageOut;
}



#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark "Private" functions
//-------------------------------------------------------------------
#endif

unsigned char toGrayAvg_(const unsigned char* rgba)
{
	return static_cast<unsigned char>((rgba[0] + rgba[1] + rgba[2])/3);
}

/**
 * Returns the max value among ther red, green, blue channels of a pixel
 * @param rgba	pointer to the start of an RGBA32 pixel
 */
unsigned char toGrayMax_(const unsigned char* rgba)
{
	return static_cast<unsigned char>(max(rgba[0], max(rgba[1], rgba[2])));
}

