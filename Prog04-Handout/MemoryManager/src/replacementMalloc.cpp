#include <cstdlib>
#include <cstdio>
#include "replacementManager.h"
#include "heapSize.h"
#include "mallocChoice.h"
#include "heapSize.h"

extern "C" {
//version 1
static int s_initialized = 0;
/**
 * @brief Ensure the memory manager is initialized
 */
static void ensure_init() {
    if (!s_initialized) {
        ReplacementManager::init(HEAPSIZE);
        s_initialized = 1;
    }
}

/**
 * @brief Initialize the memory manager
 */
void mm_init() {
    ensure_init();
}

/**
 * @brief Shutdown the memory manager
 */
void mm_shutdown() {
    if (s_initialized) {
        ReplacementManager::shutdown(REPLACEMENT_MANAGER_VERBOSE);
        s_initialized = 0;
    }
}
/**
 * @brief Allocate memory of size n bytes
 * @param n Number of bytes to allocate
 * @return void* Pointer to the allocated memory, or nullptr on failure
 */
void* myMalloc(std::size_t n) {
    ensure_init();
    void* p = nullptr;
    // Runtime algorithm selection
    const int algo = ALGORITHM_CHOICE;
    if (algo == FIRST_FIT) {
        p = ReplacementManager::alloc_first_fit(n);
    } else if (algo == BEST_FIT) {
        p = ReplacementManager::alloc_best_fit(n);
    } else {
        // default fallback is first-fit if an unknown choice is set
        p = ReplacementManager::alloc_first_fit(n);
    }

    if (REPLACEMENT_MANAGER_VERBOSE) {
        std::printf("malloc(%zu) -> %p\n", n, p);
        ReplacementManager::print_map();
    }
    return p;
}
/**
 * @brief Free previously allocated memory
 * @param p Pointer to the memory to free
 */
void printHeapMap() {
    ensure_init();
    ReplacementManager::print_map();
}

//version 2
/**
 * @brief Allocate a handle for nElems * elemSize bytes; returns NULL on failure
 * @param nElems Number of elements
 * @param elemSize Size of each element in bytes
 * @return Handle A handle to the allocated memory, or NULL on failure
 */
Handle makeHandle(size_t nElems, size_t elemSize) {
    using namespace ReplacementManager;
    std::size_t n = nElems * elemSize;
    HandleEntry* he = nullptr;
    void* p = handle_alloc_bytes(n, &he);
    if (!p) return nullptr;
    // Return a pointer to the master pointer field (pointer-to-pointer)
    return (Handle)(&he->master);
}

/**
 * @brief Free a handle and its associated memory.
 * @param h The handle to free.
 */
void hfree(Handle h) {
    using namespace ReplacementManager;
    HandleEntry* he = handle_from_user(h);
    if (!he) throw std::invalid_argument("Invalid handle");
    handle_free(he);
}
/**
 * @brief Lock a handle to prevent it from being moved by defragmentatio
 * @param h The handle to lock
 */
void lock(Handle h) {
    using namespace ReplacementManager;
    HandleEntry* he = handle_from_user(h);
    if (!he) throw std::invalid_argument("Invalid handle");
    handle_lock(he, true);
}
/**
 * @brief Unlock a handle to allow it to be moved by defragmentation
 * @param h The handle to unlock
 */
void unlock(Handle h) {
    using namespace ReplacementManager;
    HandleEntry* he = handle_from_user(h);
    if (!he) throw std::invalid_argument("Invalid handle");
    handle_lock(he, false);
}
/**
 * @brief Defragment the heap by moving unlocked handle-owned blocks to eliminate fragmentation.
 */
void defrag(void) {
    using namespace ReplacementManager;
    handle_defrag();
}
/**
 * @brief Free previously allocated memory
 * @param p Pointer to the memory to free
 */
void myFree(void* p) {
    if (p == nullptr) return; // C semantics: free(NULL) is a no-op
    ReplacementManager::dealloc(p);
}

}