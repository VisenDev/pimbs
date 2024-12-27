#if !defined(ALLOCATOR_H) /* || (defined(ALLOCATOR_IMPLEMENTATION) && !defined(ALLOCATOR_IMPLEMENTED))*/


#define ALLOCATOR_H

#ifdef ALLOCATOR_IMPLEMENTATION
    #define ALLOCATOR_IMPLEMENTED
#endif /*ALLOCATOR_IMPLEMENTATION*/

#include "attributes.h"
#include "debug.h"
#include "tui.h"
#include "strutils.h"       
#include "testing.h"
       
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

NODISCARD 
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

NODISCARD 
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



/* memcpy */
static void memory_copy(void * dest, const void * const src, const unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    char * destbuf = dest;
    const char * const srcbuf = src;
    unsigned long i = 0;
    for(i = 0; i < byte_count; ++i) {
        destbuf[i] = srcbuf[i];
    }
}
#else
;
#endif

static void test_memory_copy(TestingState * t)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    unsigned int i = 0;
    unsigned int errors = 0;
    char dest[10000];
    char src[10000];

    testing_start_test(t, "memory_copy");

    for(i = 0; i < 10000; ++i) {
        char ch = (char)(i%255);
        src[i] = ch;
    }
    memory_copy(&dest, &src, 10000);
    for(i = 0; i < 10000; ++i) {
        if(dest[i] != src[i]) {
            errors += 1;
        }
    }

    testing_expect(t, errors == 0);
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


/* fixed buffer allocation memory layout
 * MAGIC is a special number used to detect if buffer overruns have happened
 *
 *                  [byte_count number of bytes]
 * byte_count, MAGIC|   ...mem...              |MAGIC, byte_count
 *                  ^
 *                  | returned pointer
 */

typedef struct {
    char premagic[4]; 
    unsigned long byte_count;
    char postmagic[4];
} FixedBufferAllocationMetadata; 

#define MAGIC {0xDE, 0xAD, 0xBE, 0xEF}

static void verify_magic(const char magic[4]) 
#ifdef ALLOCATOR_IMPLEMENTATION
{
    const char valid[4] = MAGIC;
    unsigned int i = 0;
    tui_printf("verifying magic\n");
    for(i = 0; i < 4; ++i) {
        debug_assert(char, valid[i], ==, valid[i]);
        debug_assert(char, magic[i], ==, valid[i]);
    }

}
#else
;
#endif


static void verify_metadata(const FixedBufferAllocationMetadata metadata)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    tui_printf("verifying metadata\n");
    verify_magic(metadata.premagic);
    verify_magic(metadata.postmagic);
}
#else
;
#endif

static FixedBufferAllocationMetadata get_metadata(void * metadata_location_ptr) 
#ifdef ALLOCATOR_IMPLEMENTATION
{
    FixedBufferAllocationMetadata result = {0}; 
    tui_printf("getting metadata\n");
    memory_copy(metadata_location_ptr, &result, sizeof(FixedBufferAllocationMetadata));
    /*verify_metadata(result);*/
    return result;
}
#else
;
#endif


static void set_metadata(void * metadata_location_ptr, const unsigned long byte_count) 
#ifdef ALLOCATOR_IMPLEMENTATION
{
    FixedBufferAllocationMetadata metadata = {0}; 
    char magic[4] = MAGIC;
    verify_magic(magic);
    tui_printf1("setting metadata: %lu\n", byte_count);
    memory_copy(&metadata.premagic, &magic, 4);
    memory_copy(&metadata.postmagic, &magic, 4);
    metadata.byte_count = byte_count;
    verify_metadata(metadata);
    memory_copy(metadata_location_ptr, &metadata, sizeof(FixedBufferAllocationMetadata));
    tui_printf("verifying metadata was set correctly\n");
    verify_metadata(get_metadata(metadata_location_ptr));
}
#else
;
#endif






static void verify_fixed_buffer_allocation(void * mem)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    char * buf = mem;
    const FixedBufferAllocationMetadata start = get_metadata(buf - sizeof(FixedBufferAllocationMetadata));
    tui_printf("verifing fixed buffer allocation\n");
    verify_metadata(start);
    {
        const FixedBufferAllocationMetadata end = get_metadata(buf + start.byte_count);
        verify_metadata(end);
    }

}
#else
;
#endif

NODISCARD 
static void* fixed_buffer_alloc (struct Allocator self, unsigned long byte_count)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    FixedBufferCtx * ctx = self.ctx;
    const unsigned long needed_bytes = byte_count + (2 * sizeof(FixedBufferAllocationMetadata));
    char * top = ctx->buf + ctx->i;

    tui_printf1("Allocating %lu bytes\n", byte_count);

    if((top + needed_bytes) < ctx->end) {
        char * result = top + sizeof(FixedBufferAllocationMetadata);
        set_metadata(top, byte_count);
        tui_printf("checking metadata was set correctly\n");
        verify_metadata(get_metadata(top));

        set_metadata(result + byte_count, byte_count);
        tui_printf("checking metadata was set correctly\n");
        verify_metadata(get_metadata(result + byte_count));

        verify_fixed_buffer_allocation(result);
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
    char * mem = old_mem;


    /*bounds check*/
    simple_assert(mem != NULL, "NULL memory buffer given to realloc");
    debug_assert(long, (long)(mem - ctx->buf), >, (long)0);
    debug_assert(void_pointer, (void*)mem, <, (void*)ctx->end);
    verify_fixed_buffer_allocation(old_mem);

    (void)new_byte_count;

    /*
    {
        char * start_byte_count_ptr = mem - (sizeof(unsigned long) * 2);
        char * start_magic_ptr = start_byte_count_ptr + sizeof(unsigned long);
        unsigned long start_byte_count = 0;
        unsigned long start_magic = 0;
        unsigned long end_magic = 0;
        unsigned long end_byte_count = 0;
        const unsigned long magic = MAGIC;

        memory_copy(&start_byte_count, start_byte_count_ptr, sizeof(unsigned long));
        memory_copy(&start_magic, start_magic_ptr, sizeof(unsigned long));
        {
            char * end_magic_ptr = start_magic_ptr + start_byte_count + sizeof(unsigned long);
            char * end_byte_count_ptr = end_magic_ptr + sizeof(unsigned long);
            memory_copy(&end_magic, end_magic_ptr, sizeof(unsigned long));
            memory_copy(&end_byte_count, end_byte_count_ptr, sizeof(unsigned long));

            debug_assert(unsigned_long, start_magic, ==, magic);
            debug_assert(unsigned_long, end_magic, ==, magic);
            debug_assert(unsigned_long, end_byte_count, ==, start_byte_count);


            For now, just alloc new memory and don't check if memory can be allocated in place
            {
                char * new_mem = self.alloc(self, new_byte_count);
                unsigned long i = 0;
                if(new_mem == NULL) {
                    return NULL;
                } else {
                    for(i = 0; i < new_byte_count; ++i) {
                        new_mem[i] = mem[i];
                    }
                    return new_mem;
                }
            }

        }
    } */
    return NULL;
}
#else
;
#endif

static void fixed_buffer_free(struct Allocator self, void * mem)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    (void) self;
    (void) mem;
    /*FixedBufferCtx * ctx = self.ctx;
    char * mem_buf = mem;*/


    /*TODO implement free*/

    /*const unsigned long * byte_count = mem_buf - sizeof(unsigned long);*/
    /*simple_assert(ctx->buf - old_mem_buf > 0)
    simple_assert(old_mem_buf < ctx->end);
    if(old_mem == ctx->top_alloc) {
        ctx->next_alloc = ctx->top_alloc + new_byte_count;
        return old_mem;
        */
}
#else
;
#endif

NODISCARD PURE_FUNCTION
Allocator fixed_buffer_allocator(char * buffer, const unsigned long buflen)
#ifdef ALLOCATOR_IMPLEMENTATION
{
    void * buf = buffer;
    FixedBufferCtx * ctx = (FixedBufferCtx*)buf;
    Allocator result = {0};
    simple_assert(buflen >= 32, "buffer too small");
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

