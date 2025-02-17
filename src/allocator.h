#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "attributes.h"
#include "debug.h"
#include "tui.h"

/* utility macros for dealing with null */
#define DEREF_OR_NULL(ptr) ptr == NULL ? NULL : *ptr
#define SAFE_DEREF(ptr) (inline_assert(ptr != NULL), *ptr)

/*logging allocation wrappers*/
/*
#define log_alloc(allocator, byte_count) \
(LOG_FUNCTION("allocating %lu bytes at %s line %d\n", byte_count, __FILE__, __LINE__), allocator.alloc(allocator, byte_count))

#define log_realloc(allocator, oldmem, byte_count) \
(LOG_FUNCTION("reallocating %lu bytes at %s line %d\n", byte_count, __FILE__, __LINE__), allocator.realloc(allocator, oldmem, byte_count))
*/

/*types*/
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


#include "strutils.h"       

#ifndef NO_STDLIB
#include "stdlib.h"

/*LIBC*/

NODISCARD
static void* libc_alloc (struct Allocator self, unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    (void)self; /*ignore ctx*/
    return malloc(byte_count); 
}
#else
;
#endif

NODISCARD
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

NODISCARD PURE_FUNCTION UNUSED
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

#endif /*NO_STDLIB*/


/*LOGGING*/

/*
NODISCARD 
static void* logging_alloc (struct Allocator self, unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    Allocator child = *(Allocator *)self.ctx;
    void * bytes = child.alloc(child, byte_count);
    if(bytes == NULL) {
        tui_printf3(TUI_YELLOW"Allocating %p with %lu bytes via ctx %p failed\n", bytes, byte_count, self.ctx);
    } else {
        tui_printf3(TUI_YELLOW"Allocating %p with %lu bytes via ctx %p\n", bytes, byte_count, self.ctx);
    }
    return bytes;
}
#else
;
#endif

NODISCARD 
static void* logging_realloc (struct Allocator self, void * old_mem, unsigned long new_byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    Allocator child = *(Allocator *) self.ctx;
    void* bytes = child.realloc(child, old_mem, new_byte_count);
    tui_printf4(TUI_YELLOW"Reallocating %p to %p to store %lu total bytes via ctx %p\n", old_mem, bytes, new_byte_count, self.ctx);

    if(bytes == NULL) {
        tui_printf4(TUI_YELLOW"Reallocating %p to %p to store %lu total bytes via ctx %p failed\n", old_mem, bytes, new_byte_count, self.ctx);
    } else {
        tui_printf4(TUI_YELLOW"Reallocating %p to %p to store %lu total bytes via ctx %p\n", old_mem, bytes, new_byte_count, self.ctx);
    }
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

NODISCARD PURE_FUNCTION UNUSED
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
*/


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


NODISCARD 
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

NODISCARD 
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

    debug_assert((long)old, != ,(long)NULL);

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

UNUSED
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
    debug_assert(index, !=, -1);
    ctx->child->free(*ctx->child, mem);

    err = AllocRecords_swap(&ctx->alloc_records, (unsigned long) index, ctx->alloc_records.len - 1);
    debug_assert(err, ==, ERR_NONE);
    ctx->alloc_records.len -= 1;
}
#else
;
#endif

NODISCARD UNUSED
int leak_check_count_leaks(struct Allocator self)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    LeakCheckCtx * ctx = self.ctx;
    return (int)ctx->alloc_records.len;
}
#else
;
#endif


UNUSED
void leak_check_print_leaks(struct Allocator self)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    LeakCheckCtx * ctx = self.ctx;
    unsigned long i = 0;
    for(i = 0; i < ctx->alloc_records.len; ++i) {
        const Allocation alloc = ctx->alloc_records.items[i];
        tui_put_str("Leaked address \"");
        tui_put_long((long)alloc.ptr);
        tui_put_str("\" containing ");
        tui_put_long((long)alloc.byte_count);
        tui_put_str(" bytes\n");
    }
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

NODISCARD 
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

NODISCARD 
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



/* Fixed Buffer */
typedef struct {
    char * buf;
    char * end;
    unsigned long i;
} FixedBufferCtx;


NODISCARD PURE_FUNCTION UNUSED
static int is_mem_top(FixedBufferCtx ctx, void * mem)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    char * mem_buf = mem;
    const unsigned long byte_count = *(((unsigned long *) mem) - 1);

    /*check if old_mem is at the top*/
    return (ctx.buf + ctx.i == mem_buf + byte_count);
}
#else
;
#endif




NODISCARD 
static void* fixed_buffer_alloc (struct Allocator self, unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    FixedBufferCtx * ctx = self.ctx;
    const unsigned long alignment = ALIGNOF(unsigned long);
    const unsigned long padding_bytes = (alignment - (ctx->i % alignment)) % alignment;
    const unsigned long bytes_needed = byte_count + sizeof(unsigned long) + padding_bytes;

    /* check buffer capacity */
    if(ctx->buf + ctx->i + bytes_needed < ctx->end) {
        void * top = ctx->buf + ctx->i + padding_bytes;
        char * result = (char*)top + sizeof(unsigned long);
        ctx->i += bytes_needed;
        *(unsigned long *)top = byte_count;
        return result;
    } else {
        return NULL;
    }
}
#else
;
#endif

NODISCARD 
static void* fixed_buffer_realloc(struct Allocator self, void * old_mem, unsigned long new_byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    FixedBufferCtx * ctx = self.ctx;
    const unsigned long old_mem_byte_count = *(((unsigned long *) old_mem) - 1);
    /*check if old_mem is at the top*/
    if(is_mem_top(*ctx, old_mem)) {
        *(((unsigned long *) old_mem) - 1) = new_byte_count;
        ctx->i -= old_mem_byte_count;
        ctx->i += new_byte_count;
        return old_mem;
    } else {
        char * mem = fixed_buffer_alloc(self, new_byte_count);
        memory_copy(mem, old_mem, old_mem_byte_count); 
        return mem;
    }
}
#else
;
#endif

static void fixed_buffer_free(struct Allocator self, void * mem)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    FixedBufferCtx * ctx = self.ctx;
    if(is_mem_top(*ctx, mem)) {
        const unsigned long byte_count = *(((unsigned long *) mem) - 1);
        ctx->i -= byte_count;
    }
}
#else
;
#endif

NODISCARD PURE_FUNCTION UNUSED
Allocator fixed_buffer_allocator(char * buffer, const unsigned long buflen)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    void * buf = buffer;
    FixedBufferCtx * ctx = (FixedBufferCtx*)buf;
    Allocator result = {0};
    debug_assert((long)buflen, >=, 64);
    ctx->buf = buffer + sizeof(FixedBufferCtx);
    ctx->end = buffer + buflen;
    ctx->i = 0;

    result.ctx = ctx;
    result.alloc = &fixed_buffer_alloc;
    result.realloc = &fixed_buffer_realloc;
    result.free = &fixed_buffer_free;
    return result;
}
#else
;
#endif


#endif /*ALLOCATOR_H*/

