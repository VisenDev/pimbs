#ifndef SPARSE_SET_H
#define SPARSE_SET_H

#include "vector.h"
#include "allocator.h"

typedef struct {
    unsigned int sparse_index;
} SparseIndex;

typedef struct {
    unsigned int dense_index;
} DenseIndex;

#define IMPLEMENT_SPARSE_SET(Name, Type) \
IMPLEMENT_VECTOR(Name##SparseIndexVec, SparseIndex) \
IMPLEMENT_VECTOR(Name##DenseIndexVec, DenseIndex) \
IMPLEMENT_VECTOR(Name##Type##Vec, Type) \
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
     Name##Type##Vec_free(a, set->dense); \
     Name##SparseIndexVec_free(a, set->dense_to_sparse); \
     Name##DenseIndexVec_free(a, set->sparse); \
} \
\
void Name##_set(Allocator a, Name * set, unsigned int index) { \
    
    
    





#endif SPARSE_SET_H
