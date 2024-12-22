#include <stdlib.h>
#include "tui.h"
#include "allocator.h"

void* libc_alloc (void* ctx, unsigned long byte_count) {
    (void)ctx; //ignore ctx
    return malloc(byte_count); 
}

void* libc_realloc (void* ctx, void * old_mem, unsigned long new_byte_count) {
    (void)ctx; //ignore ctx
    return realloc(old_mem, new_byte_count); 
}

void libc_free(void* ctx, void * mem) {
    (void)ctx; //ignore ctx
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


void* logging_alloc (void* ctx, unsigned long byte_count) {
    Allocator * child = (Allocator *) ctx;
    void * bytes = child->alloc(child->ctx, byte_count);
    tui_printf(TUI_YELLOW"Allocating %p with %lu bytes via ctx %p\n", bytes, byte_count, ctx);
    return bytes;
}

void* logging_realloc (void* ctx, void * old_mem, unsigned long new_byte_count) {
    Allocator * child = (Allocator *) ctx;
    void* bytes = child->realloc(child->ctx, old_mem, new_byte_count);
    tui_printf(TUI_YELLOW"Reallocating %p to %p to store %i total bytes via ctx %p\n", old_mem, bytes, new_byte_count, ctx);
    return bytes;
}

void logging_free(void* ctx, void * mem) {
    Allocator * child = (Allocator *) ctx;
    tui_printf(TUI_YELLOW"Freeing %p via ctx %p\n", mem, ctx);
    child->free(child->ctx, mem);
}

Allocator logging_allocator(Allocator *child) {
    return (Allocator){
        .ctx = child,
        .alloc = &logging_alloc,
        .realloc = &logging_realloc,
        .free = &logging_free,
    };
}
