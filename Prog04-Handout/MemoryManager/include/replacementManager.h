#ifndef REPLACEMENT_MANAGER_H
#define REPLACEMENT_MANAGER_H

#include <cstddef>   // std::size_t
#include <list>

// Bring in the C-facing API typedef Handle so we can reference it in prototypes
#include "replacementMalloc.h"

struct HandleEntry; // forward decl so Block can have a pointer to it

// A heap block in the fake heap
struct Block {
    std::size_t offset;    // byte offset from gHeapBase
    std::size_t size;      // size in bytes
    bool        isFree;    // free or allocated
    HandleEntry* owner;    // nullptr if raw myMalloc; non-null if handle-owned
};

// A handle record that owns a movable block
struct HandleEntry {
    void* master;   // the “master pointer” the user’s Handle points to
    bool  locked;   // true => cannot be moved by defrag
    bool  alive;    // true while allocated
};

namespace ReplacementManager {

// ==== globals ====
extern unsigned char*          gHeapBase;
extern std::size_t             gHeapSize;
extern std::list<Block>        gBlocks;
extern std::list<HandleEntry>  gHandles;

// ==== core (Version 1) ====
void init(std::size_t heap_size);
void shutdown(bool verbose);
void* alloc_first_fit(std::size_t n);
void* alloc_best_fit(std::size_t n);
void  dealloc(void* p);
void  print_map();
bool  owns_pointer(void* p);

// handle helpers version 2
void*        handle_alloc_bytes(std::size_t n, HandleEntry** outOwner);
void         handle_attach_owner(Block& b, HandleEntry* he);
void         handle_free(HandleEntry* he);
void         handle_lock(HandleEntry* he, bool lockState);
void         handle_defrag();
HandleEntry* handle_from_user(Handle h);
bool         any_handles_locked();

} // namespace ReplacementManager

#endif // REPLACEMENT_MANAGER_H
