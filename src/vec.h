#include "allocator.h"
#include "concat.h"

#ifndef VEC_TYPE 
#error "VEC_TYPE must be defined before including <vector.h>"
#endif //VECTOR_TYPE
       
#ifndef VEC_NAME
#error "VEC_NAME must be defined before including <vector.h>"
#endif //VECTOR_TYPE
       
typedef struct VEC_NAME {
    VEC_TYPE * items;
    unsigned long len;
    unsigned long cap;
} VEC_NAME;

VEC_NAME CONCAT(VEC_NAME, _init) (void)
#ifdef VEC_IMPLEMENTATION
{
    return (VEC_NAME) {
        .items = NULL,
        .len = 0,
        .cap = 0,
    };
}
#else 
;
#endif


void CONCAT(VEC_NAME, _free) (Allocator a, VEC_NAME * self) 
#ifdef VEC_IMPLEMENTATION
{
    if(self->items != NULL) {
        a.free(a, self->items);
    }
    self->items = NULL;
    self->len = 0;
    self->cap = 0;
}
#else 
;
#endif

int CONCAT(VEC_NAME, _ensure_capacity) (Allocator a, VEC_NAME * self, unsigned long capacity)
#ifdef VEC_IMPLEMENTATION
{
    if(self->items == NULL) {
        VEC_TYPE * newmem = a.alloc(a, capacity * sizeof(VEC_TYPE));
        if(newmem == NULL) {
            return 1;
        } else {
            self->items = newmem;
            self->cap = capacity;
        }

    } else if(self->cap < capacity) {
        VEC_TYPE * newmem = a.realloc(a, self->items, capacity * sizeof(VEC_TYPE));
        if(newmem == NULL) {
            return 1;
        } else {
            self->items = newmem;
            self->cap = capacity;
        }

    }
    return 0;
}
#else 
;
#endif

int CONCAT(VEC_NAME, _append) (Allocator a, VEC_NAME * self, VEC_TYPE value)
#ifdef VEC_IMPLEMENTATION
{
    if(self->len >= self->cap) {
        const int err = CONCAT(VEC_NAME, _ensure_capacity)(a, self, (1 + self->cap) * 2);
        if(err != 0) {
            return err;
        } else {
            return CONCAT(VEC_NAME, _append)(a, self, value);
        }
    } else {
        const unsigned long index = self->len;
        self->items[index] = value;
        self->len += 1;
        return 0;
    }
}
#else 
;
#endif

int CONCAT(VEC_NAME, _append_n_times) (Allocator a, VEC_NAME * self, unsigned long times, VEC_TYPE value) 
#ifdef VEC_IMPLEMENTATION
{
    for(unsigned long i = 0; i < times; ++i){
        const int err = CONCAT(VEC_NAME, _append)(a, self, value);
        if(err != 0) {
            return err;
        }
    }
    return 0;
}
#else 
;
#endif


int CONCAT(VEC_NAME, _swap) (VEC_NAME * self, unsigned long lhs, unsigned long rhs) 
#ifdef VEC_IMPLEMENTATION
{
    //Bounds check
    if(lhs >= self->len || rhs >= self->len) {
        return 1;
    }

    //Do nothing if lhs and rhs are the same index
    if(lhs == rhs) {
        return 0;
    }

    const VEC_TYPE tmp = self->items[lhs];
    self->items[lhs] = self->items[rhs];
    self->items[rhs] = tmp;

    return 0;
}
#else 
;
#endif
    
#undef VEC_NAME
#undef VEC_TYPE
