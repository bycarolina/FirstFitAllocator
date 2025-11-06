#ifndef MYMEMORY_H
#define MYMEMORY_H

#include <stddef.h>
#include <stdint.h> 

typedef struct allocation {
    void *start;
    size_t size;
    struct allocation *next;
} allocation_t;

typedef struct {
    void *pool;
    size_t total_size;
    allocation_t *head;
} mymemory_t;

mymemory_t* mymemory_init(size_t size);
void*       mymemory_alloc(mymemory_t *m, size_t size);
void        mymemory_free(mymemory_t *m, void *ptr);
void        mymemory_display(mymemory_t *m);
void        mymemory_stats(mymemory_t *m);
void        mymemory_cleanup(mymemory_t *m);

#define mymemory_malloc  mymemory_alloc
#define mymemory_release mymemory_cleanup

#endif
