#include <algorithm>	//	for max
#include <cstring>		// for memcpy and strcmp
//
#include "rotate.h"

using namespace std;

#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Prototypes of "private" functions
//-------------------------------------------------------------------
#endif

/**	Produces a rotated copy of the input image (rotated by 90 degree clockwise).
 *	This function currently only works for RGBA32_RASTER images.
 *	@param	image	pointer to the RGBA32_RASTER image to rotate
 *	@return	a new image struct that stores the rotated image
 */
RasterImage rotateImage90(const RasterImage& image);

/**	Produces a rotated copy of the input image (rotated by 180 degree clockwise).
 *	This function currently only works for RGBA32_RASTER images.
 *	@param	image	pointer to the RGBA32_RASTER image to rotate
 *	@return	a new image struct that stores the rotated image
 */
RasterImage rotateImage180(const RasterImage& image);

/**	Produces a rotated copy of the input image (rotated by 270 degree clockwise).
 *	This function currently only works for RGBA32_RASTER images.
 *	@param	image	pointer to the RGBA32_RASTER image to rotate
 *	@return	a new image struct that stores the rotated image
 */
RasterImage rotateImage270(const RasterImage& image);


#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Public functions
//-------------------------------------------------------------------
#endif


RasterImage rotate(const RasterImage& image, RotationVal rotVal)
{
	RasterImage outImage;
	switch(rotVal)
	{
		case NO_ROTATION:
		{
			outImage = copyRasterImage(image);
		}
		break;
		
		case ROTATE_90:
		{
			outImage = rotateImage90(image);
		}
		break;
		
		case ROTATE_180:
		{
			outImage = rotateImage180(image);
		}
		break;
		
		case ROTATE_270:
		{
			outImage = rotateImage270(image);
		}
		break;
		
		//	do shut warnings up
		default:
			break;
	}

	return outImage;
}



#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark "Private" functions
//-------------------------------------------------------------------
#endif



//	In a rotation by 90 degree clockwise, the pixel at row i, col j in the input image
//	ends up at row outHeight - j - 1, col i in the output image
RasterImage rotateImage90(const RasterImage& image)
{
	RasterImage outImage = newImage(image.height, image.width, RGBA32_RASTER);
	int* rasterIn = (int*)(image.raster);
	int* rasterOut = (int*)(outImage.raster);

	for (unsigned i=0; i<image.height; i++)
	{
		for (unsigned j=0; j<image.width; j++)
			rasterOut[(outImage.height-j-1)*outImage.width + i] =
			rasterIn[i*image.width + j];
	}

	return outImage;
}

//	In a rotation by 180 degree clockwise, the pixel at row i, col j in the input image
//	ends up at row height - i - 1, col weight - j in the output image
RasterImage rotateImage180(const RasterImage& image)
{
	RasterImage outImage = newImage(image.width, image.height, RGBA32_RASTER);

	int* rasterIn = (int*)(image.raster);
	int* rasterOut = (int*)(outImage.raster);

	for (unsigned i=0; i<image.height; i++)
	{
		for (unsigned j=0; j<image.width; j++)
			rasterOut[(outImage.height-i-1)*outImage.width + outImage.width - j -1] =
			rasterIn[i*image.width + j];
	}

	return outImage;
}

//	In a rotation by 270 degree clockwise, the pixel at row i, col j in the input image
//	ends up at row j, col outWidth - i - 1 in the output image
RasterImage rotateImage270(const RasterImage& image)
{
	RasterImage outImage = newImage(image.height, image.width, RGBA32_RASTER);
	int* rasterIn = (int*)(image.raster);
	int* rasterOut = (int*)(outImage.raster);

	for (unsigned i=0; i<image.height; i++)
	{
		for (unsigned j=0; j<image.width; j++)
			rasterOut[j*outImage.width + outImage.width - i - 1] =
			rasterIn[i*image.width + j];
	}

	return outImage;
}
