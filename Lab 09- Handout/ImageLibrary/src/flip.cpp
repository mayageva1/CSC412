#include <algorithm>	//	for max
#include <cstring>		// for memcpy and strcmp
//
#include "crop.h"
#include "gray.h"
#include "flip.h"
#include "rotate.h"

using namespace std;


#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Public functions
//-------------------------------------------------------------------
#endif



RasterImage flipV(const RasterImage& imageIn)
{
	RasterImage imageOut = newImage(imageIn.width, imageIn.height,
									imageIn.type, 1);
	
	const unsigned char** rasterIn = static_cast<const unsigned char**>(imageIn.raster2D);
	unsigned char** rasterOut = static_cast<unsigned char**>(imageOut.raster2D);
	for (unsigned short i=0; i<imageIn.height; i++)
	{
		for (unsigned short j=0; j<imageIn.width; j++)
		{
			memcpy(rasterOut[i], rasterIn[imageIn.height-i-1], imageIn.bytesPerRow);
		}
	}

	return imageOut;
}

RasterImage flipH(const RasterImage& imageIn)
{
	RasterImage imageOut = newImage(imageIn.width, imageIn.height,
									imageIn.type, 1);
	
	switch (imageIn.type)
	{
		case GRAY_RASTER:
			{
				const unsigned char** rasterIn = static_cast<const unsigned char**>(imageIn.raster2D);
				unsigned char** rasterOut = static_cast<unsigned char**>(imageOut.raster2D);
				for (unsigned short i=0; i<imageIn.height; i++)
				{
					for (unsigned short j=0; j<imageIn.width; j++)
					{
						rasterOut[i][j] = rasterIn[i][imageIn.width-j-1];
					}
				}
			}
			break;
			
		case RGBA32_RASTER:
		case FLOAT_RASTER:
			{
				const int** rasterIn = static_cast<const int**>(imageIn.raster2D);
				int** rasterOut = static_cast<int**>(imageOut.raster2D);
				for (unsigned short i=0; i<imageIn.height; i++)
				{
					for (unsigned short j=0; j<imageIn.width; j++)
					{
						rasterOut[i][j] = rasterIn[i][imageIn.width-j-1];
					}
				}
			}
			break;
		
		//	unsupported
		default:
			exit(100);
			break;
	}
	
	return imageOut;
}

