#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "RasterImage.h"
#include "ImageIO.h"       // readImage(...) provided by the handout library
#include "operations.h"

// Case-insensitive ".tga" check
static int has_tga_ext(const char* path) {
    size_t n = strlen(path);
    if (n < 4) return 0;
    const char* ext = path + (n - 4);
    return (tolower(ext[0]) == '.' &&
            tolower(ext[1]) == 't' &&
            tolower(ext[2]) == 'g' &&
            tolower(ext[3]) == 'a');
}

// Parse a single modifier chain like "-vh", "-wv", "-h", "-w", "-v".
// Returns 0 on success, nonzero on error (and prints the required error).
// Fills out verbose/mode; ensures -w and -h are not both present.
static int parse_modifier_chain(const char* arg, int* saw_chain, int* verbose, DimMode* mode) {
    if (arg[0] != '-') return 0;   // not a modifier; ignore here
    // Only one chain allowed total
    if (*saw_chain) {
        fprintf(stderr, "Multiple modifier chains not allowed.\n");
        return 1;
    }
    *saw_chain = 1;

    int saw_w = 0, saw_h = 0, saw_v = 0;

    for (size_t i = 1; arg[i] != '\0'; ++i) {
        char c = arg[i];
        if (c == 'w') saw_w = 1;
        else if (c == 'h') saw_h = 1;
        else if (c == 'v') saw_v = 1;
        else {
            fprintf(stderr, "Invalid modifier: -%c\n", c);
            return 1;
        }
    }

    if (saw_w && saw_h) {
        fprintf(stderr, "Modifiers -w and -h cannot be used together\n");
        return 1;
    }

    *verbose = saw_v ? 1 : 0;

    if (saw_w) *mode = DM_WIDTH;
    else if (saw_h) *mode = DM_HEIGHT;
    else *mode = DM_BOTH;

    return 0;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        // Minimal usage hint (not required by spec, but helpful)
        fprintf(stderr, "Usage: %s [-h|-w][-v] <image.tga>\n", argv[0]);
        return 1;
    }

    int saw_chain = 0;
    int verbose = 0;
    DimMode mode = DM_BOTH;

    const char* imgPath = NULL;

    // Collect exactly one modifier chain and exactly one file path.
    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            // Parse the (single) chain
            if (parse_modifier_chain(argv[i], &saw_chain, &verbose, &mode) != 0) {
                return 1;
            }
        } else {
            if (imgPath != NULL) {
                // More than one non-modifier arg -> by spec we only expect one path
                // The spec doesn't define this error; safest is to mimic "invalid modifier".
                fprintf(stderr, "Multiple modifier chains not allowed.\n");
                return 1;
            }
            imgPath = argv[i];
        }
    }

    if (imgPath == NULL) {
        fprintf(stderr, "File not found: (no path provided)\n");
        return 1;
    }

    // Extension check (before trying to open)
    if (!has_tga_ext(imgPath)) {
        fprintf(stderr, "The argument is not a tga file: %s\n", imgPath);
        return 1;
    }

    // Existence/readability check (as per examples)
    FILE* f = fopen(imgPath, "rb");
    if (!f) {
        fprintf(stderr, "File not found: %s\n", imgPath);
        return 1;
    }
    fclose(f);

    // Read via the library
    RasterImage img = readImage((char*)imgPath);   // provided by ImageIO.h
    if (!img.raster) {
        fprintf(stderr, "File cannot be read %s\n", imgPath);
        return 1;
    }

    // Format result
    char* out = formatDimensions(&img, verbose, mode);
    if (!out) {
        freeImage(&img);
        fprintf(stderr, "File cannot be read %s\n", imgPath);
        return 1;
    }

    printf("%s\n", out);

    free(out);
    freeImage(&img);
    return 0;
}
