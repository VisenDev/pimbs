#include "concat.h"
#include "attributes.h"
#include "allocator.h"

#ifdef VEC_IMPLEMENTATION
    #include "errors.h"
#endif /*VEC_IMPLEMENTATION*/

/**************CHECK TYPE AND NAME*************/
#ifndef VEC_TYPE 
    #error "VEC_TYPE must be defined before including \"vec.h\""
#endif /*VECTOR_TYPE*/
       
#ifndef VEC_NAME
    #error "VEC_NAME must be defined before including \"vec.h\""
#endif /*VECTOR_TYPE*/
       

/**************IMPLEMENTATION*****************/
typedef struct VEC_NAME {
    VEC_TYPE * items;
    unsigned long len;
    unsigned long cap;
} VEC_NAME;

NODISCARD PURE_FUNCTION
VEC_NAME CONCAT(VEC_NAME, _init) (void)
#ifdef VEC_IMPLEMENTATION
{
    VEC_NAME result =  {
        NULL,
        0,
        0,
    };
    return result;
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

NODISCARD
int CONCAT(VEC_NAME, _ensure_capacity) (Allocator a, VEC_NAME * self, unsigned long capacity)
#ifdef VEC_IMPLEMENTATION
{
    if(self->items == NULL) {
        void * newmem = a.alloc(a, capacity * sizeof(VEC_TYPE));
        if(newmem == NULL) {
            return ERR_ALLOCATION_FAILURE;
        } else {
            self->items = newmem;
            self->cap = capacity;
        }

    } else if(self->cap < capacity) {
        void * newmem = a.realloc(a, self->items, capacity * sizeof(VEC_TYPE));
        if(newmem == NULL) {
            return ERR_ALLOCATION_FAILURE;
        } else {
            self->items = newmem;
            self->cap = capacity;
        }

    }
    return ERR_NONE;
}
#else 
;
#endif

NODISCARD
int CONCAT(VEC_NAME, _append) (Allocator a, VEC_NAME * self, VEC_TYPE value)
#ifdef VEC_IMPLEMENTATION
{
    simple_assert(self != NULL, "self is NULL");
    if(self->len >= self->cap) {
        const int err = CONCAT(VEC_NAME, _ensure_capacity)(a, self, (1 + self->cap) * 2);
        if(err != ERR_NONE) {
            return err;
        } else {
            return CONCAT(VEC_NAME, _append)(a, self, value);
        }
    } else {
        unsigned long index = 0;
        simple_assert(self->items != NULL, "self.items is not NULL");

        self->len += 1;
        index = self->len - 1;
        simple_assert(index < self->cap, "index is valid");
        memory_copy(&self->items[index], &value, sizeof(VEC_TYPE));
        /*self->items[index] = value;*/
        return ERR_NONE;
    }
}
#else 
;
#endif

NODISCARD
int CONCAT(VEC_NAME, _append_n_times) (Allocator a, VEC_NAME * self, unsigned long times, VEC_TYPE value) 
#ifdef VEC_IMPLEMENTATION
{
    unsigned long i = 0;
    for(i = 0; i < times; ++i){
        const int err = CONCAT(VEC_NAME, _append)(a, self, value);
        if(err != ERR_NONE) {
            return err;
        }
    }
    return ERR_NONE;
}
#else 
;
#endif


NODISCARD
int CONCAT(VEC_NAME, _swap) (VEC_NAME * self, unsigned long lhs, unsigned long rhs) 
#ifdef VEC_IMPLEMENTATION
{
    VEC_TYPE tmp;

    /*Bounds check*/
    if(lhs >= self->len || rhs >= self->len) {
        return ERR_INDEX_OUT_OF_BOUNDS;
    }

    /*Do nothing if lhs and rhs are the same index*/
    if(lhs == rhs) {
        return ERR_NONE;
    }

    tmp = self->items[lhs];
    self->items[lhs] = self->items[rhs];
    self->items[rhs] = tmp;

    return ERR_NONE;
}
#else 
;
#endif

NODISCARD 
VEC_TYPE * CONCAT(VEC_NAME, _top)(const VEC_NAME * const self)
#ifdef VEC_IMPLEMENTATION
{
    if(self->len <= 0) {
        return NULL;
    } else {
        return &self->items[self->len - 1];
    }

}
#else
;
#endif


NODISCARD 
VEC_TYPE * CONCAT(VEC_NAME, _pop)(VEC_NAME * self)
#ifdef VEC_IMPLEMENTATION
{
    VEC_TYPE * top = CONCAT(VEC_NAME, _top)(self);
    if(top == NULL) {
        return NULL;
    } else {
        self->len -= 1;
        return top;
    }

}
#else
;
#endif



NODISCARD 
VEC_TYPE * CONCAT(VEC_NAME, _swap_pop_top)(VEC_NAME * self, unsigned long index)
#ifdef VEC_IMPLEMENTATION
{
    if(self->len <= 0) {
        return 0;
    } else if (self->len == 1) {
        return CONCAT(VEC_NAME, _pop)(self);
    } else {
        const int err = CONCAT(VEC_NAME, _swap)(self, self->len - 1, index);
        if(err != ERR_NONE) {
            return NULL;
        }
        return CONCAT(VEC_NAME, _pop)(self);
    }

}
#else
;
#endif
    
#undef VEC_NAME
#undef VEC_TYPE
#undef VEC_IMPLEMENTATION
