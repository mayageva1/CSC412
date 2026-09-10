#include "replacementManager.h"
#include <cstdio>
#include <cstring>
#include <stdexcept>
#include <cassert>

namespace ReplacementManager {

unsigned char* gHeapBase = nullptr;
std::size_t    gHeapSize = 0;
std::list<Block> gBlocks;
std::list<HandleEntry> gHandles;

/**
 * @brief Convert between offsets and addresses within the managed heap.
 * @param offset Byte offset from gHeapBase
 * @return void* Corresponding pointer within the heap
 */
static void* addressFromOffset(std::size_t offset) {
    return static_cast<void*>(gHeapBase + offset);
}

/**
 * @brief Convert between addresses and offsets within the managed heap.
 * @param p Pointer within the heap
 * @return std::size_t Byte offset from gHeapBase
 */
static std::size_t offsetFromAddress(void* p) {
    return static_cast<unsigned char*>(p) - gHeapBase;
}

// version 1
/**
 * @brief Check if a pointer is owned by the managed heap.
 * @param p Pointer to check
 * @return true if the pointer is owned, false otherwise
 */
bool owns_pointer(void* p) {
    if (gHeapBase == nullptr) return false;
    unsigned char* up = static_cast<unsigned char*>(p);
    return up >= gHeapBase && up < (gHeapBase + gHeapSize);
}

/*
 * @brief Validate the state of the managed heap
 * @param when Context string for error reporting
 */
static void validate_heap(const char* when) {
    // Sorted, in-range, non-overlapping, positive sizes
    std::size_t prevEnd = 0;
    bool first = true;
    for (const auto& b : gBlocks) {
        // in range
        assert(b.size > 0);
        assert(b.offset + b.size <= gHeapSize);

        // sorted & non-overlapping
        if (!first) assert(b.offset >= prevEnd);
        first = false;
        prevEnd = b.offset + b.size;
    }
    // optional: ensure there are no adjacent free blocks left un-coalesced
    auto it = gBlocks.begin();
    auto nx = it; if (nx != gBlocks.end()) ++nx;
    for (; nx != gBlocks.end(); ++it, ++nx) {
        assert(!(it->isFree && nx->isFree &&
                 it->offset + it->size == nx->offset));
    }
    (void)when; // for breakpoint/printf 
}

//merge a free block with free neighbors if they are adjacent
/*
 * @brief Merge a free block with adjacent free neighbors
 * @param it Iterator to the block to coalesce
 */
void coalesce_neighbors(std::list<Block>::iterator it) {
    if (it != gBlocks.begin()) {
        auto prev = std::prev(it);
        if (prev->isFree && it->isFree &&
            prev->offset + prev->size == it->offset) {
            prev->size += it->size;
            it = gBlocks.erase(it);
            it = prev;
        }
    }
    auto next = std::next(it);
    if (next != gBlocks.end() &&
        it->isFree && next->isFree &&
        it->offset + it->size == next->offset) {
        it->size += next->size;
        gBlocks.erase(next);
    }
}

/**
 * @brief Initialize the custom heap region
 *  allocates a large buffer and sets up
 * the initial free block
 * @return int 0 on success, nonzero on failure
 */
void init(std::size_t heap_size) {
    gHeapBase = new unsigned char[heap_size];
    gHeapSize = heap_size;
    gBlocks.clear();
    Block initialBlock = {0, heap_size, true, nullptr};
    gBlocks.push_back(initialBlock);
    validate_heap("after init");
}

/**
 * @brief Shutdown the custom heap region
 * @param verbose If true, print the heap map before shutdown
 */
void shutdown(bool verbose) {
    if (verbose) print_map();

    bool leak = false;
    for (const auto& b : gBlocks) if (!b.isFree) { leak = true; break; }
    if (leak) {
        std::fprintf(stderr, "[MM] Leaks detected:\n");
        for (const auto& b : gBlocks)
            if (!b.isFree)
                std::fprintf(stderr, "  block off=%zu size=%zu\n", b.offset, b.size);
    }
    if (any_handles_locked())
        std::fprintf(stderr, "[MM] Warning: some handles still locked at shutdown.\n");

    delete[] gHeapBase;
    gHeapBase = nullptr;
    gHeapSize = 0;
    gBlocks.clear();
    gHandles.clear();
}

//allocation strategies
/**
 * @brief Allocate memory using the first-fit strategy
 * @param n Number of bytes to allocate
 * @return void* Pointer to the allocated memory, or nullptr on failure
 */
void* alloc_first_fit(std::size_t n) {
    for (auto it = gBlocks.begin(); it != gBlocks.end(); ++it) {
        if (it->isFree && it->size >= n) {
            if (it->size > n) {
                Block tail{ it->offset + n, it->size - n, true, nullptr }; // owner=null
                it->size = n;
                gBlocks.insert(std::next(it), tail);
            }
            it->isFree = false;
            it->owner  = nullptr;   
            validate_heap("after first-fit alloc");
            return addressFromOffset(it->offset);
        }
    }
    validate_heap("after failed first-fit alloc");
    return nullptr;
}
/**/
/**
 * @brief Allocate memory using the best-fit strategy
 * @param n Number of bytes to allocat
 * @return void* Pointer to the allocated memory, or nullptr on failure
 */
void* alloc_best_fit(std::size_t n) {
    auto best = gBlocks.end();
    for (auto it = gBlocks.begin(); it != gBlocks.end(); ++it) {
        if (it->isFree && it->size >= n) {
            if (best == gBlocks.end() || it->size < best->size) best = it;
        }
    }
    if (best == gBlocks.end()) {
        validate_heap("after failed best-fit alloc");
        return nullptr;
    }
    if (best->size > n) {
        Block tail{ best->offset + n, best->size - n, true, nullptr }; // owner=null
        best->size = n;
        gBlocks.insert(std::next(best), tail);
    }
    best->isFree = false;
    best->owner  = nullptr;  // <-- IMPORTANT
    validate_heap("after best-fit alloc");
    return addressFromOffset(best->offset);
}

//deallocation
/**
 * @brief Deallocate a previously allocated memory block
 * @param p Pointer to the memory block to deallocate
 */
void dealloc(void* p) {
    if (!owns_pointer(p)) {
        std::fprintf(stderr, "[MM] free error: pointer not in heap %p\n", p);
        std::abort();
    }
    std::size_t off = offsetFromAddress(p);
    for (auto it = gBlocks.begin(); it != gBlocks.end(); ++it) {
        if (it->offset == off) {
            if (it->isFree) {
                std::fprintf(stderr, "[MM] double free at offset %zu\n", off);
                std::abort();
            }
            it->isFree = true;
            it->owner  = nullptr;
            coalesce_neighbors(it);
            validate_heap("after free");
            return;
        }
        // defensive: detect interior free attempt (undefined in C, but catch it)
        if (!it->isFree && off > it->offset && off < it->offset + it->size) {
            std::fprintf(stderr, "[MM] free error: interior pointer %p (block start %zu size %zu)\n",
                         p, it->offset, it->size);
            std::abort();
        }
    }
    validate_heap("after failed free");
    std::fprintf(stderr, "free error: no matching block for %p\n", p);
    std::abort();
}


//print heap map
/**
 * @brief Print the current state of the heap
 */
void print_map() {
    printf("Heap Map:\n");
    for (const auto& block : gBlocks) {
        printf("Offset: %10zu | Size: %10zu | %s\n",
            block.offset, block.size,
            block.isFree ? "Free" : "Allocated");
    }
}

// Version 2
/*
 * @brief Get the handle entry associated with a user handle
 * @param h The user handle to lookup
 * @return HandleEntry* Pointer to the associated handle entry, or nullptr if not found
 */
HandleEntry* handle_from_user(Handle h) {
    if (!h) return nullptr;
    void** pMaster = (void**)h; // our Handle is &entry.master
    for (auto& he : gHandles) {
        if (&he.master == pMaster) return &he;
    }
    return nullptr;
}
/**/
/**
 * @brief Attach a handle entry to a memory block
 * @param b The memory block to modify
 * @param he The handle entry to attach
 */
void handle_attach_owner(Block& b, HandleEntry* entry) {
    b.owner   = entry;
    entry->master = addressFromOffset(b.offset);
    entry->alive = true;
}

// Allocate n bytes for a handle, tag the block’s owner, return raw pointer
/**
 * @brief Allocate a memory block for a handle
 * This function allocates a memory block of at least n bytes and associates it with a handle
 */
void* handle_alloc_bytes(std::size_t n, HandleEntry** outOwner) {
    // Use your existing strategy; here I call first-fit directly
    void* p = alloc_first_fit(n);
    if (!p) return nullptr;

    // Find the block at that offset to tag owner
    std::size_t off = offsetFromAddress(p);
    for (auto it = gBlocks.begin(); it != gBlocks.end(); ++it) {
        if (!it->isFree && it->offset == off && it->size == n) {
            gHandles.push_back({p, false, true});
            HandleEntry* he = &gHandles.back();
            it->owner = he;
            if (outOwner) *outOwner = he;
            return p;
        }
    }
    return nullptr;
}
/**
 * @brief Free a handle and its associated memory block
 * @param he The handle entry to free
 */
void handle_free(HandleEntry* he) {
    if (!he || !he->alive) throw std::invalid_argument("Invalid handle");
    void* p = he->master;
    if (!owns_pointer(p)) throw std::invalid_argument("Handle master not in heap");
    std::size_t off = offsetFromAddress(p);
    for (auto it = gBlocks.begin(); it != gBlocks.end(); ++it) {
        if (!it->isFree && it->offset == off) {
            it->isFree = true;
            it->owner  = nullptr;
            he->alive  = false;
            coalesce_neighbors(it);
            return;
        }
    }
    throw std::invalid_argument("Handle master does not match any block");
}

/**
 * @brief Lock or unlock a handle to prevent or allow movement during defragmentation
 * @param he The handle entry to modify
 * @param lockState True to lock the handle, false to unlock
 */
void handle_lock(HandleEntry* he, bool lockState) {
    if (!he || !he->alive) throw std::invalid_argument("Invalid handle");
    he->locked = lockState;
}

/**
 * @brief Defragment the heap by moving unlocked handle-owned blocks to eliminate fragmentation
 */
void handle_defrag() {
    // 1) Sort by offset so we scan left-to-right
    gBlocks.sort([](const Block& a, const Block& b){ return a.offset < b.offset; });

    // 2) Move movable (unlocked handle-owned) allocated blocks to the left, packing against 'cursor'
    std::size_t cursor = 0;
    for (auto it = gBlocks.begin(); it != gBlocks.end(); ++it) {
        if (it->isFree) continue;

        bool movable = (it->owner != nullptr) && !it->owner->locked;
        if (!movable) {
            // pin cursor to cover this fixed block
            if (cursor < it->offset + it->size) cursor = it->offset + it->size;
            continue;
        }

        if (it->offset != cursor) {
            void* src = addressFromOffset(it->offset);
            void* dst = addressFromOffset(cursor);
            std::memmove(dst, src, it->size);
            it->offset = cursor;
            it->owner->master = dst; // update master pointer
        }
        cursor += it->size;
    }

    // 3) Gather allocated blocks (with updated offsets), sorted by offset
    std::vector<Block> allocs;
    allocs.reserve(gBlocks.size());
    for (const auto& b : gBlocks) {
        if (!b.isFree) allocs.push_back(b);
    }
    std::sort(allocs.begin(), allocs.end(),
              [](const Block& a, const Block& b){ return a.offset < b.offset; });

    // 4) Rebuild the block list with explicit free gaps
    gBlocks.clear();
    std::size_t pos = 0;

    // leading free gap
    if (!allocs.empty() && allocs.front().offset > 0) {
        gBlocks.push_back({ 0, allocs.front().offset, true, nullptr });
        pos = allocs.front().offset;
    }

    // alternate: allocated block then the free gap to the next allocated block
    for (std::size_t i = 0; i < allocs.size(); ++i) {
        const Block& a = allocs[i];
        // if there is an unexpected hole before this alloc, add free gap
        if (pos < a.offset) {
            gBlocks.push_back({ pos, a.offset - pos, true, nullptr });
            pos = a.offset;
        }
        gBlocks.push_back(a);
        pos = a.offset + a.size;

        if (i + 1 < allocs.size()) {
            const Block& nxt = allocs[i+1];
            if (pos < nxt.offset) {
                gBlocks.push_back({ pos, nxt.offset - pos, true, nullptr });
                pos = nxt.offset;
            }
        }
    }

    // final tail free gap
    if (pos < gHeapSize) {
        gBlocks.push_back({ pos, gHeapSize - pos, true, nullptr });
    }

    validate_heap("after defrag");
}

/**
 * @brief Check if any handles are currently locked
 * @return true if any handles are locked, false otherwise
 */
bool any_handles_locked() {
    for (auto& he : gHandles) if (he.alive && he.locked) return true;
    return false;
}

} // namespace ReplacementManager
