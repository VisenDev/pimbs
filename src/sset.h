
#include "attributes.h"

#ifdef SSET_IMPLEMENTATION
    #include "concat.h"
    #include "allocator.h"
    #include "tui.h"
    #include "debug.h"
#endif

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

#ifdef SSET_IMPLEMENTATION
   #define VEC_IMPLEMENTATION
#endif //SSET_IMPLEMENTATION
#define Sparse CONCAT(SSET_NAME, _sparse)
#define VEC_NAME Sparse 
#define VEC_TYPE long
#include "vec.h"

#ifdef SSET_IMPLEMENTATION
   #define VEC_IMPLEMENTATION
#endif //SSET_IMPLEMENTATION
#define DenseToSparse CONCAT(SSET_NAME, _dense_to_sparse)
#define VEC_NAME DenseToSparse
#define VEC_TYPE unsigned long
#include "vec.h"

#undef VEC_IMPLEMENTATION

#define NULL_INDEX -1

typedef struct SSET_NAME {
    Dense dense;
    Sparse sparse;
    DenseToSparse dense_to_sparse;
} SSET_NAME;

NODISCARD
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

NODISCARD 
int CONCAT(SSET_NAME, _expand_sparse) (Allocator a, SSET_NAME * self, unsigned long required_length)
#ifdef SSET_IMPLEMENTATION
{
    if(self->sparse.len < required_length) {
        return CONCAT(Sparse, _append_n_times)(a, &self->sparse, (required_length - self->sparse.len) + 1, NULL_INDEX);
    }
    return 0;
}
#else
;
#endif


NODISCARD 
int CONCAT(SSET_NAME, _put)(Allocator a, SSET_NAME * self, unsigned long index, SSET_TYPE value) 
#ifdef SSET_IMPLEMENTATION
{

    int err = 0;

    if(index >= self->sparse.len) {
        err = CONCAT(SSET_NAME, _expand_sparse)(a, self, index + 1);
        if (err != 0) {
            return err;
        }
    }

    debug_assert(unsigned_long, index, <, self->sparse.len);

    const long dense_index = self->sparse.items[index];

    if(dense_index == NULL_INDEX) {
        //append new
        err = CONCAT(Dense, _append)(a, &self->dense, value);
        if(err != 0) {
            return err;
        }
        err = CONCAT(DenseToSparse, _append)(a, &self->dense_to_sparse, index);
        if(err != 0) {
            return err;
        }
        self->sparse.items[index] = (long)self->dense.len - 1;
    } else {
        //replace existing
        self->dense.items[dense_index] = value;
    }

    return 0;
}
#else 
;
#endif

NODISCARD
int CONCAT(SSET_NAME, _delete)(SSET_NAME * self, const unsigned long index) 
#ifdef SSET_IMPLEMENTATION
{
    int err = 0;


    //bounds checks
    if(index >= self->sparse.len) {
        debug_printf("attempting to delete index %lu from sparse array of length %lu\n", index, self->sparse.len);
        return 1;
    }

    const long dense_index = self->sparse.items[index];
    if(dense_index == NULL_INDEX) {
        debug_printf("attempting delete NULL_INDEX index %lu\n", index);
        return 1;
    }

    //Swap top and index
    err = CONCAT(Dense, _swap)(&self->dense, (unsigned long)dense_index, self->dense.len - 1); 
    if(err != 0) {
        debug_printf("Swapping dense_index %ld and top index %lu failed\n", dense_index, self->dense.len - 1);
        debug_printf("dense len: %lu\n", self->dense.len);
        debug_printf("sset.h line %d\n", __LINE__);
        return err;
    }
    err = CONCAT(DenseToSparse, _swap)(&self->dense_to_sparse, (unsigned long)dense_index, self->dense.len - 1); 
    if(err != 0) {
        debug_printf("Swapping dense_to_sparse indexes %ld and %lu failed\n", dense_index, self->dense.len - 1);
        debug_printf("dense len: %lu\n", self->dense.len);
        return err;
    }

    //reduce len of dense and dense_to_sparse
    self->dense.len -= 1;
    self->dense_to_sparse.len -= 1;


    //adjust the sparse index previously referring to the top element
    self->sparse.items[self->dense_to_sparse.items[dense_index]] = dense_index;

    //unset sparse array
    self->sparse.items[index] = NULL_INDEX;

    return 0;
}
#else
;
#endif

NODISCARD PURE_FUNCTION
SSET_TYPE * CONCAT(SSET_NAME, _get)(SSET_NAME * self, const unsigned long index) 
#ifdef SSET_IMPLEMENTATION
{
    //bounds checks
    if(index >= self->sparse.len) {
        return NULL;
    }

    const long dense_index = self->sparse.items[index];
    if(dense_index == NULL_INDEX) {
        return NULL;
    }

    if((unsigned long)dense_index >= self->dense.len) {
        debug_printf("attempting to access dense_index %ld in array of length %lu\n", dense_index, self->dense.len);
    }
    return &self->dense.items[dense_index];
}
#else
;
#endif




#undef SSET_NAME
#undef SSET_TYPE
#undef SSET_IMPLEMENTATION
