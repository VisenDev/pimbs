#include <stdlib.h>
#include "tui.h"
#include "allocator.h"

#define ARENA_IMPLEMENTATION
#include "3rdparty/arena.h"

void* libc_alloc (struct Allocator self, unsigned long byte_count) {
    (void)self; //ignore ctx
    return malloc(byte_count); 
}

void* libc_realloc (struct Allocator self, void * old_mem, unsigned long new_byte_count) {
    (void)self; //ignore ctx
    return realloc(old_mem, new_byte_count); 
}

void libc_free(struct Allocator self, void * mem) {
    (void)self; //ignore ctx
    free(mem); 
}

Allocator libc_allocator(void) {
    return (Allocator){
        .ctx = NULL,
        .alloc = &libc_alloc,
        .realloc = &libc_realloc,
        .free = &libc_free,
    };
}


void* logging_alloc (struct Allocator self, unsigned long byte_count) {
    Allocator child = *(Allocator *)self.ctx;
    void * bytes = child.alloc(child, byte_count);
    tui_printf(TUI_YELLOW"Allocating %p with %lu bytes via ctx %p\n", bytes, byte_count, self.ctx);
    return bytes;
}

void* logging_realloc (struct Allocator self, void * old_mem, unsigned long new_byte_count) {
    Allocator child = *(Allocator *) self.ctx;
    void* bytes = child.realloc(child, old_mem, new_byte_count);
    tui_printf(TUI_YELLOW"Reallocating %p to %p to store %i total bytes via ctx %p\n", old_mem, bytes, new_byte_count, self.ctx);
    return bytes;
}

void logging_free(struct Allocator self, void * mem) {
    Allocator child = *(Allocator *) self.ctx;
    tui_printf(TUI_YELLOW"Freeing %p via ctx %p\n", mem, self.ctx);
    child.free(child, mem);
}

Allocator logging_allocator(Allocator *child) {
    return (Allocator){
        .ctx = child,
        .alloc = &logging_alloc,
        .realloc = &logging_realloc,
        .free = &logging_free,
    };
}


//ARENA

typedef struct {
    void * ptr;
    unsigned long byte_count;
} Allocation;

#define VEC_TYPE Allocation
#define VEC_NAME AllocRecords
#define VEC_IMPLEMENTATION
#include "vec.h"

typedef struct {
    Arena arena;
    Allocator libc;
    AllocRecords allocs;
} TsodingArenaCtx;

void* tsoding_arena_alloc(struct Allocator self, unsigned long byte_count) {
    TsodingArenaCtx * ctx = self.ctx;
    void * mem =  arena_alloc(&ctx->arena, (size_t)byte_count);
    AllocRecords_append(ctx->libc, &ctx->allocs, (Allocation){.ptr = mem, .byte_count = byte_count});
    return mem;
}


void* tsoding_arena_realloc (struct Allocator self, void * old_mem, unsigned long new_byte_count) {
    TsodingArenaCtx * ctx = self.ctx;

    Allocation * old = NULL;
    for(unsigned long i = 0; i < ctx->allocs.len; ++i){
        if(ctx->allocs.items[i].ptr == old_mem) {
            old = &ctx->allocs.items[i];
            break;    
        }
    }

    assert(old != NULL && "old_mem pointer was not found");

    void * new_mem = arena_realloc(&ctx->arena, old_mem, old->byte_count, new_byte_count);
    if(new_mem == NULL) {
        return NULL;
    } else {
        old->ptr = new_mem;
        old->byte_count = new_byte_count;
    }
    return new_mem;
}


void tsoding_arena_free(struct Allocator self, void * mem) {
    (void) self;
    (void) mem;
}

Allocator tsoding_arena_allocator(void) {
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

void tsoding_arena_allocator_free(Allocator * a) {
    TsodingArenaCtx * ctx = a->ctx;
    arena_free(&ctx->arena); 
    AllocRecords_free(ctx->libc, &ctx->allocs);
    free(ctx);
}

