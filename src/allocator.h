#if !defined(ALLOCATOR_H) || (defined(ALLOCATOR_IMPLEMENTATION) && !defined(ALLOCATOR_IMPLEMENTED))

#define ALLOCATOR_H
#ifdef ALLOCATOR_IMPLEMENTATION
    #define ALLOCATOR_IMPLEMENTED
#endif //ALLOCATOR_IMPLEMENTATION
    

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


//Allocator libc_allocator(void);
//Allocator logging_allocator(Allocator *child);
//
//Allocator tsoding_arena_allocator(void);
//void tsoding_arena_allocator_free(Allocator *);
//
//void leak_check_allocator_free(struct Allocator self);
//Allocator leak_check_allocator(struct Allocator child);
//int leak_check_count_leaks(struct Allocator self);

#include <stdlib.h>
#include <string.h>
#include "debug.h"

#ifdef ALLOCATOR_IMPLEMENTATION
   #define TUI_IMPLEMENTATION
#endif
#include "tui.h"

#ifdef ALLOCATOR_IMPLEMENTATION
   #define ARENA_IMPLEMENTATION
#endif
#include "3rdparty/arena.h"


//UTILS
static inline char * string_copy(Allocator a, const char * const str, size_t maxlen)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    const size_t len = strnlen(str, maxlen);
    char * buf = a.alloc(a, sizeof(char) * (len + 1));
    memcpy(buf, str, len + 1);
    if(buf == NULL) {
        return NULL;
    }

    debug_assert(char, buf[len], ==, 0);
    debug_assert(int, strncmp(buf, str, len), ==, 0);
    return buf;
}
#else
;
#endif


//LIBC
//#ifndef NO_CSTDLIB TODO

void* libc_alloc (struct Allocator self, unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    (void)self; //ignore ctx
    return malloc(byte_count); 
}
#else
;
#endif

void* libc_realloc (struct Allocator self, void * old_mem, unsigned long new_byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    (void)self; //ignore ctx
    return realloc(old_mem, new_byte_count); 
}
#else
;
#endif

void libc_free(struct Allocator self, void * mem)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    (void)self; //ignore ctx
    free(mem); 
}
#else
;
#endif

Allocator libc_allocator(void)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    return (Allocator){
        .ctx = NULL,
        .alloc = &libc_alloc,
        .realloc = &libc_realloc,
        .free = &libc_free,
    };
}
#else
;
#endif


//LOGGING

void* logging_alloc (struct Allocator self, unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    Allocator child = *(Allocator *)self.ctx;
    void * bytes = child.alloc(child, byte_count);
    tui_printf(TUI_YELLOW"Allocating %p with %lu bytes via ctx %p\n", bytes, byte_count, self.ctx);
    return bytes;
}
#else
;
#endif

void* logging_realloc (struct Allocator self, void * old_mem, unsigned long new_byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    Allocator child = *(Allocator *) self.ctx;
    void* bytes = child.realloc(child, old_mem, new_byte_count);
    tui_printf(TUI_YELLOW"Reallocating %p to %p to store %lu total bytes via ctx %p\n", old_mem, bytes, new_byte_count, self.ctx);
    return bytes;
}
#else
;
#endif

void logging_free(struct Allocator self, void * mem)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    Allocator child = *(Allocator *) self.ctx;
    tui_printf(TUI_YELLOW"Freeing %p via ctx %p\n", mem, self.ctx);
    child.free(child, mem);
}
#else
;
#endif

Allocator logging_allocator(Allocator *child)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    return (Allocator){
        .ctx = child,
        .alloc = &logging_alloc,
        .realloc = &logging_realloc,
        .free = &logging_free,
    };
}
#else
;
#endif


//ARENA

typedef struct {
    void * ptr;
    unsigned long byte_count;
} Allocation;

#define VEC_TYPE Allocation
#define VEC_NAME AllocRecords

#if defined(ALLOCATOR_IMPLEMENTATION) && !defined(VEC_IMPLEMENTATION)
    #define VEC_IMPLEMENTATION
    #include "vec.h"
#else
    #include "vec.h"
#endif


typedef struct {
    Arena arena;
    Allocator libc;
    AllocRecords allocs;
} TsodingArenaCtx;

void* tsoding_arena_alloc(struct Allocator self, unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    TsodingArenaCtx * ctx = self.ctx;
    void * mem =  arena_alloc(&ctx->arena, (size_t)byte_count);
    AllocRecords_append(ctx->libc, &ctx->allocs, (Allocation){.ptr = mem, .byte_count = byte_count});
    return mem;
}
#else
;
#endif


void* tsoding_arena_realloc (struct Allocator self, void * old_mem, unsigned long new_byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    TsodingArenaCtx * ctx = self.ctx;

    Allocation * old = NULL;
    for(unsigned long i = 0; i < ctx->allocs.len; ++i){
        if(ctx->allocs.items[i].ptr == old_mem) {
            old = &ctx->allocs.items[i];
            break;    
        }
    }

    debug_assert(void_pointer, (void*)old, !=, NULL);

    void * new_mem = arena_realloc(&ctx->arena, old_mem, old->byte_count, new_byte_count);
    if(new_mem == NULL) {
        return NULL;
    } else {
        old->ptr = new_mem;
        old->byte_count = new_byte_count;
    }
    return new_mem;
}
#else
;
#endif


void tsoding_arena_free(struct Allocator self, void * mem)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    (void) self;
    (void) mem;
}
#else
;
#endif

Allocator tsoding_arena_allocator(void)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    TsodingArenaCtx * ctx = malloc(sizeof(TsodingArenaCtx));
    ctx->arena = (Arena){0};
    ctx->allocs = AllocRecords_init();
    ctx->libc = libc_allocator();

    return (Allocator){
        .ctx = ctx,
        .alloc = &tsoding_arena_alloc,
        .realloc = &tsoding_arena_realloc,
        .free = &tsoding_arena_free,
    };
}
#else
;
#endif

void tsoding_arena_allocator_free(Allocator * a)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    TsodingArenaCtx * ctx = a->ctx;
    arena_free(&ctx->arena); 
    AllocRecords_free(ctx->libc, &ctx->allocs);
    free(ctx);
}
#else
;
#endif


//LEAKCHECK
typedef struct {
    Allocator child;
    AllocRecords alloc_records;
} LeakCheckCtx;


void * leak_check_alloc(struct Allocator self, unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    LeakCheckCtx * ctx = self.ctx;
    void * mem = ctx->child.alloc(ctx->child, byte_count); 
    const Allocation record = (Allocation){.ptr = mem, .byte_count = byte_count};
    AllocRecords_append(ctx->child, &ctx->alloc_records, record);
    return mem;
}
#else
;
#endif

void * leak_check_realloc (struct Allocator self, void * old_mem, unsigned long new_byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    LeakCheckCtx * ctx = self.ctx;

    Allocation * old = NULL;
    for(unsigned long i = 0; i < ctx->alloc_records.len; ++i){
        if(ctx->alloc_records.items[i].ptr == old_mem) {
            old = &ctx->alloc_records.items[i];
            break;    
        }
    }

    assert(old != NULL && "old_mem pointer was not found");

    void * new_mem = ctx->child.realloc(ctx->child, old_mem, new_byte_count);
    if(new_mem == NULL) {
        return NULL;
    } else {
        old->ptr = new_mem;
        old->byte_count = new_byte_count;
    }
    return new_mem;
}
#else
;
#endif

void leak_check_free (struct Allocator self, void * mem)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    LeakCheckCtx * ctx = self.ctx;
    Allocation * old = NULL;
    unsigned long index = 0;

    for(unsigned long i = 0; i < ctx->alloc_records.len; ++i){
        if(ctx->alloc_records.items[i].ptr == mem) {
            old = &ctx->alloc_records.items[i];
            index = i;
            break;    
        }
    }
    assert(old != NULL && "mem pointer was not found");
    ctx->child.free(ctx->child, mem);

    AllocRecords_swap(&ctx->alloc_records, index, ctx->alloc_records.len - 1);
    ctx->alloc_records.len -= 1;
}
#else
;
#endif

int leak_check_count_leaks(struct Allocator self)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    LeakCheckCtx * ctx = self.ctx;
    return (int)ctx->alloc_records.len;
}
#else
;
#endif

Allocator leak_check_allocator(struct Allocator child)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    LeakCheckCtx * ctx = child.alloc(child, sizeof(LeakCheckCtx));
    ctx->alloc_records = AllocRecords_init();
    ctx->child = child;

    return (Allocator){
        .ctx = ctx,
        .alloc = &leak_check_alloc,
        .realloc = &leak_check_realloc,
        .free = &leak_check_free,
    };
}
#else
;
#endif


void leak_check_allocator_free(struct Allocator self)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    LeakCheckCtx * ctx = self.ctx;
    ctx->child.free(ctx->child, ctx);
}
#else
;
#endif


#endif //ALLOCATOR_H
