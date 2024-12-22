#ifndef ALLOCATOR_H
#define ALLOCATOR_H

typedef void* (*AllocFn)(void*,unsigned long);
typedef void* (*ReallocFn)(void*,void*,unsigned long);
typedef void  (*FreeFn)(void*,void*);

typedef struct {
    void * ctx;
    AllocFn alloc;
    ReallocFn realloc;
    FreeFn free;
} Allocator;

Allocator libc_allocator();
Allocator logging_allocator(Allocator *child);

#endif //ALLOCATOR_H
