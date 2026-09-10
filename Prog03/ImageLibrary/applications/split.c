// Applications/split.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "RasterImage.h"
#include "ImageIO.h"  // readImage, writeImage

// --- minimal helpers ---
static int has_tga_ext(const char* p) {
    size_t n = strlen(p);
    if (n < 4) return 0;
    const char* e = p + n - 4;
    return (tolower(e[0]) == '.' && tolower(e[1]) == 't' &&
            tolower(e[2]) == 'g' && tolower(e[3]) == 'a');
}

static void make_out_path(char* dst, size_t dstsz,
                          const char* outDir, const char* inPath, const char* tag) {
    // base name (after last slash/backslash)
    const char* base = inPath;
    const char* s1 = strrchr(inPath, '/');
#ifdef _WIN32
    const char* s2 = strrchr(inPath, '\\');
    if (s2 && (!s1 || s2 > s1)) s1 = s2;
#endif
    if (s1) base = s1 + 1;

    // strip extension (".tga"/".TGA"/etc.)
    const char* dot = strrchr(base, '.');
    size_t stem_len = dot ? (size_t)(dot - base) : strlen(base);

    // ensure one trailing slash for outDir
    size_t od_len = strlen(outDir);
    int needs_slash = (od_len == 0 ||
                       (outDir[od_len-1] != '/' 
#ifdef _WIN32
                        && outDir[od_len-1] != '\\'
#endif
                       ));

    if (needs_slash) {
        snprintf(dst, dstsz, "%s/%.*s [%s].tga", outDir, (int)stem_len, base, tag);
    } else {
        snprintf(dst, dstsz, "%s%.*s [%s].tga",  outDir, (int)stem_len, base, tag);
    }
}

static RasterImage extract_gray(const RasterImage* in, int ch /*0=R,1=G,2=BB*/) {
    RasterImage out = newImage(in->width, in->height, GRAY_RASTER);
    if (!out.raster) return out;

    const unsigned int* rowIn = (const unsigned int*) in->raster;
    unsigned char*      rowOut= (unsigned char*) out.raster;

    for (unsigned short i = 0; i < in->height; i++) {
        for (unsigned short j = 0; j < in->width; j++) {
            const unsigned char* rgba = (const unsigned char*) (rowIn + j);
            rowOut[j] = rgba[ch];  // bytes 0,1,2 correspond to R,G,B in this codebase
        }
        rowIn  += in->width;
        rowOut += out.bytesPerRow; // == width for GRAY_RASTER
    }
    return out;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <image.tga> <output_dir>\n", argv[0]);
        return 1;
    }

    const char* inPath = argv[1];
    const char* outDir = argv[2];

    // Spec-required errors:
    if (!has_tga_ext(inPath)) {
        fprintf(stderr, "The argument is not a tga file: %s\n", inPath);
        return 1;
    }
    FILE* test = fopen(inPath, "rb");
    if (!test) {
        fprintf(stderr, "File not found: %s\n", inPath);
        return 1;
    }
    fclose(test);

    // Read input
    RasterImage img = readImage((char*) inPath);
    if (!img.raster) {
        fprintf(stderr, "File cannot be read %s\n", inPath);
        return 1;
    }

    char outR[1024], outG[1024], outB[1024];
    make_out_path(outR, sizeof(outR), outDir, inPath, "r");
    make_out_path(outG, sizeof(outG), outDir, inPath, "g");
    make_out_path(outB, sizeof(outB), outDir, inPath, "b");

    int rc = 0;

    if (img.type == GRAY_RASTER) {
        // Just clone the gray image three times (they'll be identical)
        RasterImage c1 = newImage(img.width, img.height, GRAY_RASTER);
        RasterImage c2 = newImage(img.width, img.height, GRAY_RASTER);
        RasterImage c3 = newImage(img.width, img.height, GRAY_RASTER);
        if (!c1.raster || !c2.raster || !c3.raster) {
            fprintf(stderr, "Unable to allocate memory\n");
            if (c1.raster) freeImage(&c1);
            if (c2.raster) freeImage(&c2);
            if (c3.raster) freeImage(&c3);
            freeImage(&img);
            return 1;
        }
        size_t bytes = (size_t)img.height * img.bytesPerRow;
        memcpy(c1.raster, img.raster, bytes);
        memcpy(c2.raster, img.raster, bytes);
        memcpy(c3.raster, img.raster, bytes);

        rc |= writeImage(&c1, (char*)outR);
        rc |= writeImage(&c2, (char*)outG);
        rc |= writeImage(&c3, (char*)outB);

        freeImage(&c1);
        freeImage(&c2);
        freeImage(&c3);
    } else if (img.type == RGBA32_RASTER) {
        RasterImage rImg = extract_gray(&img, 0);
        RasterImage gImg = extract_gray(&img, 1);
        RasterImage bImg = extract_gray(&img, 2);

        if (!rImg.raster || !gImg.raster || !bImg.raster) {
            fprintf(stderr, "Unable to allocate memory\n");
            if (rImg.raster) freeImage(&rImg);
            if (gImg.raster) freeImage(&gImg);
            if (bImg.raster) freeImage(&bImg);
            freeImage(&img);
            return 1;
        }

        rc |= writeImage(&rImg, (char*)outR);
        rc |= writeImage(&gImg, (char*)outG);
        rc |= writeImage(&bImg, (char*)outB);

        freeImage(&rImg);
        freeImage(&gImg);
        freeImage(&bImg);
    } else {
        fprintf(stderr, "Unsupported image type\n");
        freeImage(&img);
        return 1;
    }

    freeImage(&img);
    if (rc != 0) {
        fprintf(stderr, "Error writing output images\n");
        return 1;
    }
    return 0;
}
