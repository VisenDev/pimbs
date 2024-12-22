#include <stdlib.h>
#include "tui.h"
#include "allocator.h"

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
