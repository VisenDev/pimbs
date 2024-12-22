#include <stdlib.h>
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

Allocator libc_allocator() {
    return (Allocator){
        .ctx = NULL,
        .alloc = &libc_alloc,
        .realloc = &libc_realloc,
        .free = &libc_free,
    };
}
