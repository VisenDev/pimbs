
#include "attributes.h"
#include "concat.h"
#include "allocator.h"

#ifdef SSET_IMPLEMENTATION
    #include "tui.h"
    #include "debug.h"
#endif

#ifndef SSET_TYPE 
#error "SSET_TYPE must be defined before including \"sset.h\""
#endif /*SSET TYPE*/
       
#ifndef SSET_NAME
#error "SSET_NAME must be defined before including \"sset.h\""
#endif /*SSET_NAME*/
       
/* Subtypes */
#ifdef SSET_IMPLEMENTATION
   #define VEC_IMPLEMENTATION
#endif /*SSET_IMPLEMENTATION*/
#define Dense CONCAT(SSET_NAME, _dense)
#define VEC_NAME Dense
#define VEC_TYPE SSET_TYPE
#include "vec.h"

#ifdef SSET_IMPLEMENTATION
   #define VEC_IMPLEMENTATION
#endif /*SSET_IMPLEMENTATION*/
#define Sparse CONCAT(SSET_NAME, _sparse)
#define VEC_NAME Sparse 
#define VEC_TYPE long
#include "vec.h"

#ifdef SSET_IMPLEMENTATION
   #define VEC_IMPLEMENTATION
#endif /*SSET_IMPLEMENTATION*/
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
    SSET_NAME result = {0};
    result.dense = CONCAT(Dense, _init)();
    result.sparse = CONCAT(Sparse, _init)();
    result.dense_to_sparse = CONCAT(DenseToSparse, _init)();
    return result;
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
SSET_TYPE * CONCAT(SSET_NAME, _get)(SSET_NAME * self, const unsigned long index) 
#ifdef SSET_IMPLEMENTATION
{

    long dense_index = NULL_INDEX;

    /*bounds checks*/
    if(index >= self->sparse.len) {
        return NULL;
    }

    dense_index = self->sparse.items[index];
    if(dense_index == NULL_INDEX) {
        return NULL;
    }

    simple_assert((unsigned long)dense_index < self->dense.len, "Stored dense index is invalid");
    return &self->dense.items[dense_index];
}
#else
;
#endif


NODISCARD 
int CONCAT(SSET_NAME, _put)(Allocator a, SSET_NAME * self, unsigned long index, SSET_TYPE value) 
#ifdef SSET_IMPLEMENTATION
{

    int err = 0;
    long dense_index = NULL_INDEX;

    /*bounds check*/
    if(index >= self->sparse.len) {
        err = CONCAT(SSET_NAME, _expand_sparse)(a, self, index + 1);
        if (err != ERR_NONE) {
            log_location();
            tui_printf2("Putting sparse set index %lu failed because %s\n", index, error_name(err));
            return err;
        }
    }

    /*assert bounds check*/
    debug_assert(unsigned_long, index, <, self->sparse.len);

    dense_index = self->sparse.items[index];

    if(dense_index == NULL_INDEX) {
        /*append new*/
        err = CONCAT(Dense, _append)(a, &self->dense, value);
        if(err != ERR_NONE) {
            log_location();
            tui_printf2("Putting sparse set index %lu failed because %s\n", index, error_name(err));
            return err;
        }
        err = CONCAT(DenseToSparse, _append)(a, &self->dense_to_sparse, index);
        if(err != ERR_NONE) {
            log_location();
            tui_printf2("Putting sparse set index %lu failed because %s\n", index, error_name(err));
            return err;
        }
        /*update sparse*/
        self->sparse.items[index] = (long)self->dense.len - 1;
    } else {
        /*replace existing*/
        tui_printf2("sset put replacing existing value at dense index %ld and normal index %lu\n", dense_index, index);
        self->dense.items[dense_index] = value;
    }

    simple_assert(CONCAT(SSET_NAME, _get)(self, index) != NULL, "index that was just put should not be null");
    return ERR_NONE;
}
#else 
;
#endif

NODISCARD
int CONCAT(SSET_NAME, _delete)(SSET_NAME * self, const unsigned long index) 
#ifdef SSET_IMPLEMENTATION
{
    int err = ERR_NONE;
    long dense_index = NULL_INDEX;


    /*bounds checks*/
    if(index >= self->sparse.len) {
        return ERR_INDEX_OUT_OF_BOUNDS;
    }

    dense_index = self->sparse.items[index];
    if(dense_index == NULL_INDEX) {
        return ERR_NOT_FOUND;
    }

    /*Swap top and index*/
    err = CONCAT(Dense, _swap)(&self->dense, (unsigned long)dense_index, self->dense.len - 1); 
    if(err != 0) {
        return err;
    }
    err = CONCAT(DenseToSparse, _swap)(&self->dense_to_sparse, (unsigned long)dense_index, self->dense.len - 1); 
    if(err != 0) {
        return err;
    }

    /*reduce len of dense and dense_to_sparse*/
    self->dense.len -= 1;
    self->dense_to_sparse.len -= 1;


    /*adjust the sparse index previously referring to the top element*/
    self->sparse.items[self->dense_to_sparse.items[dense_index]] = dense_index;

    /*unset sparse array*/
    self->sparse.items[index] = NULL_INDEX;

    return ERR_NONE;
}
#else
;
#endif


NODISCARD 
SSET_TYPE * CONCAT(SSET_NAME, _get_or_put)(Allocator a, SSET_NAME * self, const unsigned long index, SSET_TYPE fallback_value)
#ifdef SSET_IMPLEMENTATION
{
    SSET_TYPE * found = CONCAT(SSET_NAME, _get)(self, index);
    if(found == NULL) {
        int err = CONCAT(SSET_NAME, _put)(a, self, index, fallback_value);
        if(err != ERR_NONE) {
            log_location();
            tui_printf2("get or set sparse set index %lu failed because %s\n", index, error_name(err));
            return NULL;
        } 
        found = CONCAT(SSET_NAME, _get)(self, index);
        simple_assert(found != NULL, "found should not be null once it has been set");
        return found;
    } else {
        return found;
    }
}
#else
;
#endif



#undef SSET_NAME
#undef SSET_TYPE
#undef SSET_IMPLEMENTATION
