#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "RasterImage.h"
#include "ImageIO.h"
#include "operations.h"

// Returns 1 if path ends with .tga or .TGA (case-insensitive), else 0
static int has_tga_ext(const char* path) {
    size_t n = strlen(path);
    if (n < 4) return 0;                 // too short to contain ".tga"

    const char* e = path + (n - 4);      // points to last 4 chars

    // Must be: '.', then 't'/'T', 'g'/'G', 'a'/'A'
    if (e[0] != '.') return 0;

    int is_t = (e[1] == 't' || e[1] == 'T');
    int is_g = (e[2] == 'g' || e[2] == 'G');
    int is_a = (e[3] == 'a' || e[3] == 'A');

    return (is_t && is_g && is_a) ? 1 : 0;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        // Return 0 (not equal) when used incorrectly.
        fprintf(stderr, "Usage: %s <image1.tga> <image2.tga>\n", argv[0]);
        return 0;
    }

    const char* p1 = argv[1];
    const char* p2 = argv[2];

    if (!has_tga_ext(p1) || !has_tga_ext(p2)) {
        return 0;
    }

    RasterImage a = readImage((char*)p1);
    if (!a.raster) { return 0; }

    RasterImage b = readImage((char*)p2);
    if (!b.raster) { freeImage(&a); return 0; }

    int same = compareImagesExact(&a, &b);

    freeImage(&a);
    freeImage(&b);

    // Per spec: return 1 if identical, 0 otherwise.
    if (same == 1) {
        return 1;
    } 
    else {
        return 0;
    }
}
