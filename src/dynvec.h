#include "debug.h"


void dvec_expand_internal(void ** items, long * len, long * cap, const unsigned long elem_size) {
    inline_assert(items != NULL);
    inline_assert(len != NULL);
    inline_assert(cap != NULL);
    inline_assert(elem_size != 0);
    inline_assert(*len >= 0);
    inline_assert(*cap >= 0);

    if(*cap <= 0 || *items == NULL) {
        *cap = 8; 
        *len = 0; 
        *items = malloc(elem_size * (unsigned long)*cap);
    } 
    if(*len >= *cap) { 
        *cap = (*cap + 1) * 2;
        *items = realloc(*items, elem_size * (unsigned long)*cap);
    }
    *len += 1;
}


#define dvec_push(self, value) \
    (dvec_expand_internal((void**)&self.items, &self.len, &self.cap, sizeof(self.elem)), self.items[self.len - 1] = value)


#define DynVec(T) struct { T * items; long len; long cap; T elem; }

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

#define dvec_get(self, i) \
    self.items[i]


#define dvec_pop(self) \
    (--self.len, self.items[self.len])

#define dvec_free(self) free(self.items)
