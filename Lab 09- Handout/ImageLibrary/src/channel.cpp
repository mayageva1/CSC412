#include "channel.h"

#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Public functions
//-------------------------------------------------------------------
#endif


void killRedChannel(RasterImage& image)
{
	int* raster = (int*)(image.raster);
	
	for (unsigned int i=0, mirrorI=image.height-1; i<image.height; i++, mirrorI--)
	{
		for (unsigned int j=0; j<image.width; j++)
		{
			//	x86 CPUs are small-endian, therefore the order of bytes in an int
			//	is inverted.  So within the int it looks as if we have ABGR
			//	
			//									  preserve -----------+-+-+
			//														  | | |
			//														  A B G R
			raster[i*image.width + j] = raster[i*image.width + j] & 0xFFFFFF00;
			//																|
			//										  kill -----------------+
		}
	}		
}
void killGreenChannel(RasterImage& image)
{
	int* raster = (int*)(image.raster);
	
	for (unsigned int i=0, mirrorI=image.height-1; i<image.height; i++, mirrorI--)
	{
		for (unsigned int j=0; j<image.width; j++)
		{
			//	x86 CPUs are small-endian, therefore the order of bytes in an int
			//	is inverted.  So within the int it looks as if we have ABGR
			//	
			//									  preserve -----------+-+---+
			//														  | |   |
			//														  A B G R
			raster[i*image.width + j] = raster[i*image.width + j] & 0xFFFF00FF;
			//															  |
			//										  kill ---------------+
		}
	}		
}

void killBlueChannel(RasterImage& image)
{
	int* raster = (int*)(image.raster);
	
	for (unsigned int i=0, mirrorI=image.height-1; i<image.height; i++, mirrorI--)
	{
		for (unsigned int j=0; j<image.width; j++)
		{
			//	x86 CPUs are small-endian, therefore the order of bytes in an int
			//	is inverted.  So within the int it looks as if we have ABGR
			//	
			//									  preserve -----------+---+-+
			//														  |   | |
			//														  A B G R
			raster[i*image.width + j] = raster[i*image.width + j] & 0xFF00FFFF;
			//															|
			//										  kill -------------+
		}
	}		
}

