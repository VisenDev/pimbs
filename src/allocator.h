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

Allocator tsoding_arena_allocator(void);
void tsoding_arena_allocator_free(Allocator *);

void leak_check_allocator_free(struct Allocator self);
Allocator leak_check_allocator(struct Allocator child);
int leak_check_count_leaks(struct Allocator self);

#endif //ALLOCATOR_H
