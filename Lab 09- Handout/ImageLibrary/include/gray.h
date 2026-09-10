#ifndef GRAY_H
#define GRAY_H

#include "RasterImage.h"

/**
 * Converts an image to gray-level values
 * 
 * @param imageIn	pointer to the image to convert to gray
 * @return a gray version of the input image
 * 
 */
RasterImage toGray(const RasterImage& imageIn);

/**
 * Converts an image to gray-level values (multithreaded version)
 * 
 * @param imageIn		pointer to the image to convert to gray
 * @param numThreads	the number of threads over which to split the work
 * @return a gray version of the input image
 * 
 */
RasterImage toGray_thr(const RasterImage& imageIn, unsigned int numThreads);

#endif	//	GRAY_H
