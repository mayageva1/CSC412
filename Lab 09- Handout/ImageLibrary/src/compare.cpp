#include <algorithm>	//	for max
#include <cstring>		// for memcpy and strcmp
//
#include "comp.h"

using namespace std;


#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Public functions
//-------------------------------------------------------------------
#endif


bool compareImages(const RasterImage& image1, const RasterImage& image2)
{
	//	Note:
	//	I am of the "a function has a single entry point and therefore,
	//	as much as possible, should have a single exit (return point), except
	//	when it completely obfuscates the code, which is not the case here, IMHO.
	bool isSame = true;
	
	//	Only look at pixel data if type and dimensions are the same
	if (image1.type == image2.type &&
		image1.width == image2.width &&
		image1.height == image2.height)
	{
		switch(image1.type)
		{
			case RGBA32_RASTER:
			//	When declaring variables within a switch case,
			{
				int** raster1 = static_cast<int**>(image1.raster2D);
				int** raster2 = static_cast<int**>(image2.raster2D);
				for (unsigned short i = 0; isSame && i < image1.height; i++)
				{
					for (unsigned int j = 0; isSame && j < image1.width; j++)
					{
						isSame = (raster1[i][j] == raster2[i][j]);
					}
				}
			}
			break;
			
			case GRAY_RASTER:
			{
				unsigned char** raster1 = static_cast<unsigned char**>(image1.raster2D);
				unsigned char** raster2 = static_cast<unsigned char**>(image2.raster2D);
				for (unsigned short i = 0; isSame && i < image1.height; i++)
				{
					for (unsigned int j = 0; isSame && j < image1.width; j++)
					{
						isSame = (raster1[i][j] == raster2[i][j]);
					}
				}

			}
			break;
			
			case FLOAT_RASTER:
			{
				float** raster1 = static_cast<float**>(image1.raster2D);
				float** raster2 = static_cast<float**>(image2.raster2D);
				for (unsigned short i = 0; isSame && i < image1.height; i++)
				{
					for (unsigned int j = 0; isSame && j < image1.width; j++)
					{
						isSame = (raster1[i][j] == raster2[i][j]);
					}
				}

			}
			break;
			
			default:
				isSame = false;
				
			break;
		}
	}
	else
	{
		isSame = false;
	}
	
   return !isSame;
}
