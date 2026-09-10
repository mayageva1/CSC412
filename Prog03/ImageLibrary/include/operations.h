#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <stdbool.h>
#include "RasterImage.h"

typedef enum {
    DM_BOTH = 0,
    DM_WIDTH,
    DM_HEIGHT
} DimMode;

char* formatDimensions(const RasterImage* img, bool verbose, DimMode mode);

// --- Exact byte-wise image comparison ---
// Returns 1 if a and b have same width/height/type and identical raster bytes; else 0.
// If either pointer is NULL or rasters are NULL, returns 0.
int compareImagesExact(const RasterImage* a, const RasterImage* b);


#endif
