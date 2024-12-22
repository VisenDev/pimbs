#ifndef SPARSE_SET_H
#define SPARSE_SET_H

#include "tui.h"
#include "vector.h"
#include "allocator.h"
#include "assert.h"

typedef struct {
    unsigned long sparse_index;
} SparseIndex;

typedef struct {
    bool valid;
    unsigned long dense_index;
} DenseIndex;

#define IMPLEMENT_SPARSE_SET(Name, Type) \
\
IMPLEMENT_VECTOR(Name##SparseIndexVec, SparseIndex) \
IMPLEMENT_VECTOR(Name##DenseIndexVec, DenseIndex) \
IMPLEMENT_VECTOR(Name##Type##Vec, Type) \
\
typedef struct { \
    Name##Type##Vec dense; \
    Name##SparseIndexVec dense_to_sparse; \
    Name##DenseIndexVec sparse; \
} Name; \
\
Name Name##_init(Allocator a) { \
    return (Name) { \
        .dense = Name##Type##Vec_init(a), \
        .dense_to_sparse= Name##SparseIndexVec_init(a), \
        .sparse = Name##DenseIndexVec_init(a), \
    }; \
} \
\
void Name##_deinit(Allocator a, Name * set) { \
     Name##Type##Vec_free(a, &set->dense); \
     Name##SparseIndexVec_free(a, &set->dense_to_sparse); \
     Name##DenseIndexVec_free(a, &set->sparse); \
} \
\
void Name##_expand_sparse(Allocator a, Name * set, unsigned int minimum_len) { \
    while(set->sparse.len <= minimum_len + 1){\
        Name##DenseIndexVec_append(a, &set->sparse, (DenseIndex){.valid = false}); \
    } \
} \
\
void Name##_set(Allocator a, Name * set, const unsigned int index, const Type value) { \
    DenseIndex * dindex = Name##DenseIndexVec_get(set->sparse, index); \
    if(dindex == NULL) { \
        Name##_expand_sparse(a, set, index); \
        Name##_set(a, set, index, value); \
        return; \
    } \
    if(dindex->valid == false) { \
        dindex->valid = true; \
        dindex->dense_index = set->dense.len; \
        Name##Type##Vec_append(a, &set->dense, value); \
        Name##SparseIndexVec_append(a, &set->dense_to_sparse, (SparseIndex){.sparse_index = index}); \
    } else { \
        assert(set->dense.len < dindex->dense_index); \
        set->dense.items[dindex->dense_index] = value; \
    } \
} \
\
void Name##_unset(Name * set, unsigned int index) { \
    DenseIndex * dindex = Name##DenseIndexVec_get(set->sparse, index); \
    if(dindex == NULL) { \
        return; \
    } \
    if(dindex->valid == false) { \
        return; \
    } \
    Type top = set->dense.items[set->dense.len - 1]; \
    SparseIndex top_index = set->dense_to_sparse.items[set->dense.len - 1]; \
    set->dense.len -= 1; \
    if(top_index.sparse_index != index) { \
        set->dense.items[index] = top; \
        set->sparse.items[top_index.sparse_index] = *dindex; \
    } \
    dindex->valid = false; \
} \
\
Type * Name##_get(Name set, unsigned int index) { \
    DenseIndex * dindex = Name##DenseIndexVec_get(set.sparse, index); \
    if(dindex == NULL) { \
        return NULL; \
    } \
    if(dindex->valid == false) { \
        return NULL; \
    } \
    return &set.dense.items[dindex->dense_index]; \
} \


#endif //SPARSE_SET_H
