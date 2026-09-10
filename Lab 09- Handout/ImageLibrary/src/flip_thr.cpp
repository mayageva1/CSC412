#include <algorithm>	//	for max
#include <cstring>		// for memcpy and strcmp
//
#include "flip.h"

using namespace std;

#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Custom Data Types
//-------------------------------------------------------------------
#endif

// thread info struct
//	We don't need to pass the data into a single struct, but I like keeping
//	all the stuff needed by a struct into a nice little package.  Furthermore,
//	this allows to grow development in the OOP direction.
struct ThreadInfo {
	//	Generally, when I have 
	int index;
	//	what do we need to pass to a thread to work with??
	
};

#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Prototypes of "private" functions
//-------------------------------------------------------------------
#endif


/** Thread function for vertical flipping
 *	(incomplete prototype)
 */
void flipVThreadFunc();



#if 0
//-------------------------------------------------------------------
#pragma mark -
#pragma mark Functions implementations
//-------------------------------------------------------------------
#endif



RasterImage flipV_thr(const RasterImage& imageIn, unsigned int numThreads)
{
	RasterImage imageOut = newImage(imageIn.width, imageIn.height,
									imageIn.type, 1);
	
	//	Step 1:	Allocate and initialize your array of ThreadInfo objects
	//			One for each thread you are about to create.
	
	//	Step 2:  Create the threads				
	for (int k=0; k<numThreads; k++)
	{
	
	}
	
	//	Here the main thread has nothing to do but wait for the processing
	//	thread to finish their work
	
	//	Step 3:  Join the threads				
	for (int k=0; k<numThreads; k++)
	{
	}

	return imageOut;
}


void flipVThreadFunc()
{
	//	For memory, I copy the code of the single-threaded function, which
	//	operates over the entire image

// 	
// 	const unsigned char** rasterIn = static_cast<const unsigned char**>(imageIn.raster2D);
// 	unsigned char** rasterOut = static_cast<unsigned char**>(imageOut.raster2D);
// 	for (unsigned short i=0; i<imageIn.height; i++)
// 	{
// 		for (unsigned short j=0; j<imageIn.width; j++)
// 		{
// 			memcpy(rasterOut[i], rasterIn[imageIn.height-i-1], imageIn.bytesPerRow);
// 		}
// 	}
}
