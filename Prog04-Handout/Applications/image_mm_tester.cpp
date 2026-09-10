#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <string>

// route malloc/free to your manager + expose mm_init/mm_shutdown/printHeapMap
#include "mallocChoice.h"

// Your ImageLibrary headers are C — wrap in extern "C"
extern "C" {
  #include "RasterImage.h"
  #include "ImageIO.h"   // declares: RasterImage readImage(char*), int writeImage(RasterImage*, char*)
}

// helper: app-controlled allocations to intentionally fragment the heap
static void* scratch(std::size_t n) { return malloc(n); }

static void usage(const char* prog) {
  std::fprintf(stderr, "Usage: %s <inA.tga> <inB.tga> [out_prefix]\n", prog);
}
static void* xmalloc(size_t n, const char* label) {
    void* p = malloc(n);
    if (!p) {
        std::fprintf(stderr, "[tester] OOM allocating %s (%zu bytes)\n", label, n);
        std::exit(1);
    }
    return p;
}
int main(int argc, char** argv) {
#if USE_REPLACEMENT_MANAGER
  mm_init();
#endif

  if (argc < 3) { usage(argv[0]); return 2; }
  const char* inA = argv[1];
  const char* inB = argv[2];
  const char* outPrefix = (argc >= 4 ? argv[3] : "out");

  // Load two images (allocator activity via readImage)
  RasterImage A = readImage(const_cast<char*>(inA));
  RasterImage B = readImage(const_cast<char*>(inB));
  if (A.raster == nullptr || B.raster == nullptr) {
    std::fprintf(stderr, "readImage failed\n");
#if USE_REPLACEMENT_MANAGER
    mm_shutdown();
#endif
    return 1;
  }

  // Fragmentation via app allocations
  void* s1 = scratch(1600);
  void* s2 = scratch(900);

  // 3) Library allocations: copy images (allocates new rasters internally)
  RasterImage A_copy = copyRasterImage(&A);
  RasterImage B_copy = copyRasterImage(&B);

  // free middle to create hole
  free(s2);
  void* s3 = scratch(780);      // split the ~900 hole
  void* s4 = scratch(640);

  // Create new images + assign rasters 
  //  (uses newImage + setRaster, so memory flows through your manager)
  RasterImage R1 = newImage(A.width, A.height, A.type);
  RasterImage R2 = newImage(B.width, B.height, B.type);

  // Replace R1/R2 rasters with our own allocated buffers (setRaster frees the old if any)
  std::size_t r1Bytes = (std::size_t)R1.bytesPerRow * R1.height;
  std::size_t r2Bytes = (std::size_t)R2.bytesPerRow * R2.height;
  void* r1buf = xmalloc(r1Bytes, "R1 raster");
  void* r2buf = xmalloc(r2Bytes, "R2 raster");
  setRaster(&R1, r1buf);
  setRaster(&R2, r2buf);

  // Simple row-copy transforms to exercise bytesPerRow (no crop needed)
  // Copy A into R1 (clamped to min dimensions)
  unsigned short rows = (A.height < R1.height) ? A.height : R1.height;
  unsigned short cols = (A.width  < R1.width ) ? A.width  : R1.width;
  unsigned int bpp = R1.bytesPerPixel;
  for (unsigned short r = 0; r < rows; ++r) {
    unsigned char* dst = (unsigned char*)R1.raster + (size_t)r * R1.bytesPerRow;
    unsigned char* src = (unsigned char*)A.raster  + (size_t)r * A.bytesPerRow;
    std::memcpy(dst, src, (size_t)cols * bpp);
  }

  // Copy B into R2 (reverse row order just to do something different)
  rows = (B.height < R2.height) ? B.height : R2.height;
  cols = (B.width  < R2.width ) ? B.width  : R2.width;
  bpp  = R2.bytesPerPixel;
  for (unsigned short r = 0; r < rows; ++r) {
    unsigned char* dst = (unsigned char*)R2.raster + (size_t)r * R2.bytesPerRow;
    unsigned char* src = (unsigned char*)B.raster  + (size_t)(rows - 1 - r) * B.bytesPerRow;
    std::memcpy(dst, src, (size_t)cols * bpp);
  }

  // Another big free to open a front hole, then a large alloc to observe fit strategy
  free(s1);
  void* s5 = scratch(1200);

  //Write results 
  std::string out1 = std::string(outPrefix) + "_copyA.tga";
  std::string out2 = std::string(outPrefix) + "_copyB_fliprows.tga";
  int rc1 = writeImage(&R1, const_cast<char*>(out1.c_str()));
  int rc2 = writeImage(&R2, const_cast<char*>(out2.c_str()));
  if (rc1 || rc2) std::fprintf(stderr, "writeImage returned non-zero (rc1=%d rc2=%d)\n", rc1, rc2);

  //Free in non-LIFO order to maximize fragmentation, then coalesce
  free(s3);
  freeImage(&A_copy);
  free(s5);
  freeImage(&B_copy);
  free(s4);
  freeImage(&R1);
  freeImage(&R2);
  freeImage(&A);
  freeImage(&B);

  std::vector<void*> small;
  small.reserve(48);
  for (int i = 0; i < 48; ++i) small.push_back(scratch(64 + (i % 6) * 16));
  for (int i = 0; i < 48; i += 2) { free(small[i]); small[i] = nullptr; }
  for (int i = 1; i < 48; i += 2) if (small[i]) free(small[i]);

  printHeapMap();   // final snapshot

#if USE_REPLACEMENT_MANAGER
  mm_shutdown();
#endif
  return 0;
}
