#ifndef FLIP_H
#define FLIP_H

#include "RasterImage.h"


/**
 * Flips an image vertically (symmetry about the middle horizontal axis)
 *
 * @param image	The image to flip
 * @return	a new image, obtained by flipping the input image vertically
 */
RasterImage flipV(const RasterImage& image);

/**
 * Flips an image vertically (symmetry about the middle horizontal axis)
 *
 *  (multithreaded version)
 *
 * @param image	The image to flip
 * @param numThreads	the number of threads over which to split the work
 * @return	a new image, obtained by flipping the input image vertically
 */
RasterImage flipV_thr(const RasterImage& image, unsigned int numThreads);

/**
 * Flips an image horizontally (symmetry about the middle vertical axis)
 * 
 * @param image	The image to flip
 * @return	a new image, obtained by flipping the input image horizontally
 */
RasterImage flipH(const RasterImage& image);



#endif	// FLIP_H
