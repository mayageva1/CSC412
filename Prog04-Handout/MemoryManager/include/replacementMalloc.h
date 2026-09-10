#ifndef REPLACEMENT_MALLOC_H
#define REPLACEMENT_MALLOC_H


#include <stddef.h>  

#ifdef __cplusplus
extern "C" {
#endif

/*Version 1) /
void   mm_init(void);
void   mm_shutdown(void);
void*  myMalloc(size_t n);
void   myFree(void* p);
void   printHeapMap(void);

/* handles api version 2 */
typedef void* Handle;

/* allocate a handle for nElems * elemSize bytes; returns NULL on failure */
Handle makeHandle(size_t nElems, size_t elemSize);

/* free a handle (and its underlying block). Error if invalid or double-free */
void   hfree(Handle h);

/* lock/unlock: prevent/allow the block from being moved by defrag() */
void   lock(Handle h);
void   unlock(Handle h);

/* compact the heap by moving UNLOCKED, handle-owned blocks and updating master pointers */
void   defrag(void);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* REPLACEMENT_MALLOC_H */
