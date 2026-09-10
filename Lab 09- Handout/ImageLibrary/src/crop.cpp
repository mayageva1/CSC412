#include <algorithm>	//	for max
#include <cstring>		// for memcpy and strcmp
//
#include "crop.h"
#include "gray.h"
#include "flipV.h"
#include "flipH.h"
#include "rotate.h"

using namespace std;


#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Public functions
//-------------------------------------------------------------------
#endif


RasterImage cropImage(const RasterImage& imageIn,
					  unsigned short cropCornerX, unsigned short cropCornerY,
					  unsigned short cropWidth, unsigned short cropHeight)
{
	RasterImage imageOut;
	
	switch (imageIn.type)
	{
		case RGBA32_RASTER:
		imageOut = newImage(cropWidth, cropHeight, RGBA32_RASTER, 0);
		break;
		
		case GRAY_RASTER:
		imageOut = newImage(cropWidth, cropHeight, GRAY_RASTER, 0);
		break;

		case FLOAT_RASTER:
		imageOut = newImage(cropWidth, cropHeight, FLOAT_RASTER, 0);
		break;
		
		default:
		exit(31);
		break;
	}
	
	//	Beware that the images are stored upside-down from the way we view them,
	//	So I need to invert the row indices.
	for (unsigned short i = 0; i<cropHeight; i++)
	{
		//	version using the 1D rasters
		const unsigned char* inRow = static_cast<unsigned char*>(imageIn.raster) +
									(imageIn.height - i - cropCornerY - 1)*imageIn.bytesPerRow;
		unsigned char* outRow = static_cast<unsigned char*>(imageOut.raster) +
								(imageOut.height - i - 1)*imageOut.bytesPerRow;
								
		memcpy(outRow, inRow + cropCornerX * imageIn.bytesPerPixel,
			   cropWidth*imageIn.bytesPerPixel);

		//	version using the 2D rasters
//		memcpy(((unsigned char**) imageOut.raster2D)[imageOut.height - i - 1],
//			   ((unsigned char**) imageIn.raster2D)[imageIn.height - i - cropCornerY - 1]
//							   					+ cropCornerX * imageIn.bytesPerPixel,
//			   cropWidth*imageIn.bytesPerPixel);
		
	}

	return imageOut;
}


