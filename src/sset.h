#include "allocator.h"
#include "concat.h"

#define SSET_TYPE int
#define SSET_NAME sset

#ifndef SSET_TYPE 
#error "SSET_TYPE must be defined before including \"sset.h\""
#endif //SSET TYPE
       
#ifndef SSET_NAME
#error "SSET_NAME must be defined before including \"sset.h\""
#endif //SSET_NAME
       
// Subtypes
#ifdef SSET_IMPLEMENTATION
#define VEC_IMPLEMENTATION
#endif //SSET_IMPLEMENTATION

#define Dense CONCAT(SSET_NAME, _dense)
#define VEC_NAME Dense
#define VEC_TYPE SSET_TYPE
#include "vec.h"

#define Sparse CONCAT(SSET_NAME, _sparse)
#define VEC_NAME Sparse 
#define VEC_TYPE long
#include "vec.h"

#define DenseToSparse CONCAT(SSET_NAME, _dense_to_sparse)
#define VEC_NAME DenseToSparse
#define VEC_TYPE unsigned long
#include "vec.h"

#undef VEC_IMPLEMENTATION

typedef struct SSET_NAME {
    Dense dense;
    Sparse sparse;
    DenseToSparse dense_to_sparse;
} SSET_NAME;

SSET_NAME CONCAT(SSET_NAME, _init) (void) 
#ifdef SSET_IMPLEMENTATION
{
    return (SSET_NAME) {
        .dense = CONCAT(Dense, _init)(),
        .sparse = CONCAT(Sparse, _init)(),
        .dense_to_sparse = CONCAT(DenseToSparse, _init)(),
    };
}
#else
;
#endif

void CONCAT(SSET_NAME, _free) (Allocator a, SSET_NAME * self)
#ifdef SSET_IMPLEMENTATION
{
    CONCAT(Dense, _free)(a, &self->dense);
    CONCAT(Sparse, _free)(a, &self->sparse);
    CONCAT(DenseToSparse, _free)(a, &self->dense_to_sparse);
}
#else
;
#endif

int CONCAT(SSET_NAME, _expand_sparse) (Allocator a, SSET_NAME * self, unsigned long required_length)
#ifdef SSET_IMPLEMENTATION
{
    if(self->sparse.len < required_length) {
        return CONCAT(Sparse, _append_n_times)(a, &self->sparse, (self->sparse.len - required_length) + 1, -1);
    }
    return 0;
}
#else
;
#endif



int CONCAT(SSET_NAME, _put)(Allocator a, SSET_NAME * self, unsigned long index, SSET_TYPE value) 
#ifdef SSET_IMPLEMENTATION
{
    if(index > self->sparse.len) {
        const int err = CONCAT(SSET_NAME, _expand_sparse)(a, self, index + 1);
        if (err != 0) {
            return err;
        }
    }

    long dense_index = self->sparse.items[index];
    if(dense_index == -1) {
        //append new
    } else {

    }

    //TODO finish
    (void) value;
    return 0;
}
#else 
;
#endif


#undef SSET_NAME
#undef SSET_TYPE
