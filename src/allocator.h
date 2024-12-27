#if !defined(ALLOCATOR_H) || (defined(ALLOCATOR_IMPLEMENTATION) && !defined(ALLOCATOR_IMPLEMENTED))
#define ALLOCATOR_H

#ifdef ALLOCATOR_IMPLEMENTATION
    #define ALLOCATOR_IMPLEMENTED
#endif /*ALLOCATOR_IMPLEMENTATION*/

#include "attributes.h"
#include "debug.h"
#include "tui.h"
       
       
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


#ifdef USE_CSTDLIB

#include "stdlib.h"

/*LIBC*/

NODISCARD MAY_ALLOCATE
static void* libc_alloc (struct Allocator self, unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    (void)self; /*ignore ctx*/
    return malloc(byte_count); 
}
#else
;
#endif

NODISCARD MAY_ALLOCATE
static void* libc_realloc (struct Allocator self, void * old_mem, unsigned long new_byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    (void)self; /*ignore ctx*/
    return realloc(old_mem, new_byte_count); 
}
#else
;
#endif

static void libc_free(struct Allocator self, void * mem)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    (void)self; /*ignore ctx*/
    free(mem); 
}
#else
;
#endif

NODISCARD PURE_FUNCTION
Allocator libc_allocator(void)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    Allocator result = {
        NULL,
        &libc_alloc,
        &libc_realloc,
        &libc_free,
    };
    return result;
}
#else
;
#endif

#endif /*USE_CSTDLIB*/


/*LOGGING*/

NODISCARD MAY_ALLOCATE
static void* logging_alloc (struct Allocator self, unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    Allocator child = *(Allocator *)self.ctx;
    void * bytes = child.alloc(child, byte_count);
    tui_printf3(TUI_YELLOW"Allocating %p with %lu bytes via ctx %p\n", bytes, byte_count, self.ctx);
    return bytes;
}
#else
;
#endif

NODISCARD MAY_ALLOCATE
static void* logging_realloc (struct Allocator self, void * old_mem, unsigned long new_byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    Allocator child = *(Allocator *) self.ctx;
    void* bytes = child.realloc(child, old_mem, new_byte_count);
    tui_printf4(TUI_YELLOW"Reallocating %p to %p to store %lu total bytes via ctx %p\n", old_mem, bytes, new_byte_count, self.ctx);
    return bytes;
}
#else
;
#endif

static void logging_free(struct Allocator self, void * mem)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    Allocator child = *(Allocator *) self.ctx;
    tui_printf2(TUI_YELLOW"Freeing %p via ctx %p\n", mem, self.ctx);
    child.free(child, mem);
}
#else
;
#endif

NODISCARD PURE_FUNCTION
Allocator logging_allocator(Allocator *child)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    Allocator result = {
        NULL,
        &logging_alloc,
        &logging_realloc,
        &logging_free,
    };
    result.ctx = child;
    return result;
}
#else
;
#endif


/*ARENA*/

typedef struct {
    void * ptr;
    unsigned long byte_count;
} Allocation;

#define VEC_TYPE Allocation
#define VEC_NAME AllocRecords

#if defined(ALLOCATOR_IMPLEMENTATION)
    #define VEC_IMPLEMENTATION
    #include "vec.h"
#else
    #include "vec.h"
#endif


/*LEAKCHECK*/
typedef struct {
    Allocator * child;
    AllocRecords alloc_records;
} LeakCheckCtx;


NODISCARD MAY_ALLOCATE
static void * leak_check_alloc(struct Allocator self, unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    LeakCheckCtx * ctx = self.ctx;
    void * mem = ctx->child->alloc(*ctx->child, byte_count); 
    int err;
    Allocation record = {0};

    record.ptr = mem;
    record.byte_count = byte_count;
    err = AllocRecords_append(*ctx->child, &ctx->alloc_records, record);
    if(err != ERR_NONE) {
        return NULL;
    }
    return mem;
}
#else
;
#endif

NODISCARD MAY_ALLOCATE
static void * leak_check_realloc (struct Allocator self, void * old_mem, unsigned long new_byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    void * new_mem = NULL;
    LeakCheckCtx * ctx = self.ctx;
    unsigned long i = 0;
    Allocation * old = NULL;

    for(i = 0; i < ctx->alloc_records.len; ++i){
        if(ctx->alloc_records.items[i].ptr == old_mem) {
            old = &ctx->alloc_records.items[i];
            break;    
        }
    }

    simple_assert(old != NULL, "old_mem pointer was not found");

    new_mem = ctx->child->realloc(*ctx->child, old_mem, new_byte_count);
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

static void leak_check_free (struct Allocator self, void * mem)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    LeakCheckCtx * ctx = self.ctx;
    int err;
    unsigned long i = 0;
    long index = -1;

    for(i = 0; i < ctx->alloc_records.len; ++i){
        if(ctx->alloc_records.items[i].ptr == mem) {
            index = (long)i;
            break;    
        }
    }
    simple_assert(index != -1 , "mem pointer was not found");
    ctx->child->free(*ctx->child, mem);

    err = AllocRecords_swap(&ctx->alloc_records, (unsigned long) index, ctx->alloc_records.len - 1);
    simple_assert(err == ERR_NONE, "Indexes for swap are incorrect");
    ctx->alloc_records.len -= 1;
}
#else
;
#endif

NODISCARD PURE_FUNCTION
static int leak_check_count_leaks(struct Allocator self)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    LeakCheckCtx * ctx = self.ctx;
    return (int)ctx->alloc_records.len;
}
#else
;
#endif

NODISCARD PURE_FUNCTION
Allocator leak_check_allocator(Allocator * child)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    Allocator result = {0};

    LeakCheckCtx * ctx = child->alloc(*child, sizeof(LeakCheckCtx));
    ctx->alloc_records = AllocRecords_init();
    ctx->child = child;

    result.ctx = ctx;
    result.alloc = &leak_check_alloc;
    result.realloc = &leak_check_realloc;
    result.free = &leak_check_free;
    return result;
}
#else
;
#endif


void leak_check_allocator_free(struct Allocator self)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    LeakCheckCtx * ctx = self.ctx;
    AllocRecords_free(*ctx->child, &ctx->alloc_records);
}
#else
;
#endif




/*ALWAYSFAILING*/

NODISCARD MAY_ALLOCATE
static void* always_failing_alloc (struct Allocator self, unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    (void)self;
    (void)byte_count;
    return NULL;
}
#else
;
#endif

NODISCARD MAY_ALLOCATE
static void* always_failing_realloc (struct Allocator self, void * old_mem, unsigned long new_byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    (void)self;
    (void)new_byte_count;
    (void)old_mem;
    return NULL;
}
#else
;
#endif

static void always_failing_free(struct Allocator self, void * mem)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    (void)self; /*ignore ctx*/
    (void)mem;
}
#else
;
#endif

NODISCARD PURE_FUNCTION
Allocator always_failing_allocator(void)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    Allocator result = {
        NULL,
        &always_failing_alloc,
        &always_failing_realloc,
        &always_failing_free,
    };
    return result;
}
#else
;
#endif


#endif /*ALLOCATOR_H*/
