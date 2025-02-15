#include "debug.h"
#include "allocator.h"


void dynvec_expand_internal(Allocator a, void ** items, long * len, long * cap, const unsigned long elem_size) {
    assert_not_null(items);
    assert_not_null(len);
    assert_not_null(cap);
    debug_assert((long)elem_size, !=, 0);
    debug_assert(*len, >=, 0);
    debug_assert(*cap, >=, 0);

    if(*cap <= 0 || *items == NULL) {
        *cap = 8; 
        *len = 0; 
        *items = a.alloc(a, elem_size * (unsigned long)*cap);
        assert_not_null(*items);
    } 
    if(*len >= *cap) { 
        *cap = (*cap + 1) * 2;
        *items = a.realloc(a, *items, elem_size * (unsigned long)*cap);
        assert_not_null(*items);
    }
    *len += 1;
}


#define dynvec_push(allocator, self, value) \
    (dynvec_expand_internal(allocator, (void**)&self.items, &self.len, &self.cap, sizeof(self.elem)), \
     self.items[self.len - 1] = value)


#define DynVec(T) struct { T * items; long len; long cap; T elem; }

/*
#define dvec_append(self, value) \
    do { \
        if(self.cap <= 0) { \
            self.cap = 8; \
            self.len = 0; \
            self.items = malloc(sizeof(self.elem) * (unsigned long)self.cap); \
        } \
        if(self.len >= self.cap) { \
            self.cap = (self.cap + 1) * 2; \
            self.items = realloc(self.items, sizeof(self.elem) * (unsigned long)self.cap); \
        } \
        self.items[self.len] = value;  \
        self.len += 1; \
    } while (0)
    */

#define dynvec_get(self, i) \
    self.items[i]


#define dynvec_pop(self) \
    (--self.len, self.items[self.len])

#define dynvec_free(allocator, self) allocator.free(allocator, self.items)
