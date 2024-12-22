#ifndef ALLOCATOR_H
#define ALLOCATOR_H

struct Allocator;

typedef void* (*AllocFn)(struct Allocator,unsigned long);
typedef void* (*ReallocFn)(struct Allocator,void*,unsigned long);
typedef void  (*FreeFn)(struct Allocator,void*);

typedef struct Allocator {
    void * ctx;
    AllocFn alloc;
    ReallocFn realloc;
    FreeFn free;
} Allocator;


Allocator libc_allocator(void);
Allocator logging_allocator(Allocator *child);

#endif //ALLOCATOR_H
