#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <cstring>
#include "mallocChoice.h"


static bool g_verbose = false;
std::vector<Handle> hlist;

int main(int argc, char** argv) {
#if USE_REPLACEMENT_MANAGER
  mm_init();
#endif


  std::vector<void*> blocks; // index-based
  for (int i = 1; i < argc; ) {
    std::string tok = argv[i++];
    if (tok == "-v") {
      g_verbose = true;
      continue;
    } 
    else if (tok == "m" && i < argc) {
      std::size_t sz = std::strtoull(argv[i++], nullptr, 10);
        
      void* p = malloc(sz); // remapped to myMalloc if replacement enabled
      if (!p) std::cout << "ALLOC FAIL " << sz << "\n";
      else {
        blocks.push_back(p);
          if (g_verbose) std::cout << "m " << sz << " -> idx " << (blocks.size()-1) << " ptr " << p << "\n";
      }

    } 
    else if (tok == "c" && i < argc) {
      std::size_t sz = std::strtoull(argv[i++], nullptr, 10);
 
    } 
    else if (tok == "f" && i < argc) {
      std::size_t idx = std::strtoull(argv[i++], nullptr, 10);
      if (idx >= blocks.size() || !blocks[idx]) {
        std::cout << "FREE INVALID INDEX " << idx << "\n";
      } else {
        free(blocks[idx]); // remapped to myFree if replacement enabled
        blocks[idx] = nullptr;
        if (g_verbose) std::cout << "f " << idx << " OK\n";
      }
    }
    else if (tok == "h" && i < argc) {
    std::size_t sz = std::strtoull(argv[i++], nullptr, 10);
    Handle h = makeHandle(1, sz);
    if (!h) std::cout << "H_ALLOC FAIL " << sz << "\n";
    else {
        hlist.push_back(h);
        if (g_verbose) std::cout << "h " << sz << " -> hidx " << (hlist.size()-1) << "\n";
    }
  }
    else if (tok == "hf" && i < argc) {
        std::size_t idx = std::strtoull(argv[i++], nullptr, 10);
        if (idx >= hlist.size() || !hlist[idx]) std::cout << "H_FREE INVALID " << idx << "\n";
        else { hfree(hlist[idx]); hlist[idx] = nullptr; if (g_verbose) std::cout << "hf " << idx << " OK\n"; }
    }
    else if (tok == "lock" && i < argc) {
        std::size_t idx = std::strtoull(argv[i++], nullptr, 10);
        if (idx >= hlist.size() || !hlist[idx]) std::cout << "LOCK INVALID " << idx << "\n";
        else { lock(hlist[idx]); if (g_verbose) std::cout << "lock " << idx << " OK\n"; }
    }
    else if (tok == "unlock" && i < argc) {
        std::size_t idx = std::strtoull(argv[i++], nullptr, 10);
        if (idx >= hlist.size() || !hlist[idx]) std::cout << "UNLOCK INVALID " << idx << "\n";
        else { unlock(hlist[idx]); if (g_verbose) std::cout << "unlock " << idx << " OK\n"; }
    }
    else if (tok == "defrag") {
        defrag();
        if (g_verbose) std::cout << "defrag OK\n";
    }
    else {
      std::cerr << "Unknown/invalid token: " << tok << "\n";
      return 2;
    }
  }

#if USE_REPLACEMENT_MANAGER
  mm_shutdown();
#endif
  return 0;
}


//g++ -std=c++17 -Wall -Wextra -O2 \
  -I . -I MemoryManager/include \
  -c MemoryManager/src/replacementManager.cpp -o replacementManager.o

//g++ -std=c++17 -Wall -Wextra -O2 \
  -I . -I MemoryManager/include \
  -c MemoryManager/src/replacementMalloc.cpp -o replacementMalloc.o


//g++ -std=c++17 -Wall -Wextra -O2 \
  -I . -I MemoryManager/include \
  Applications/prog04_01.cpp replacementManager.o replacementMalloc.o \
  -o prog04
