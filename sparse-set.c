#include <stdint.h>

#define TESTING_IMPLEMENTATION
#include "testing.h"

#define VECTOR_IMPLEMENTATION
#include "vector.h"

/*
#define SparseSet(T) \
    struct { \
        Vector(T) dense; \
        Vector(uint64_t) dense_to_sparse_map; \
        Vector(uint64_t) sparse; \
    }
    */

typedef struct {
    Vector dense;
    Vector sparse;
    Vector dense_to_sparse_map;
} SparseSet;

typedef struct {
    enum {TAG_NULL, TAG_VALID} tag;
    uint64_t data;
} OptionalIndex;

SparseSet ss_init(size_t elem_size_bytes, uint64_t initial_capacity) {
   SparseSet result = (SparseSet){
        .dense = _vector_init(elem_size_bytes),
        .sparse = vector_init(SparseValue),
        .dense_to_sparse_map = vector_init(uint64_t),
   };
   SparseValue initial_sparse_contents[initial_capacity] = {};
   vector_append_array(&result.sparse, initial_sparse_contents, initial_capacity);
   return result;
}

void ss_deinit(SparseSet * self){
    vector_deinit(&self->dense);
    vector_deinit(&self->sparse);
    vector_deinit(&self->dense_to_sparse_map);
    *self = (SparseSet){0};
}

void ss_set(SparseSet * self, void * value, uint64_t sparse_index) {
    SparseValue * dense_index = vector_get(SparseValue, self->sparse, sparse_index);
    if(dense_index->tag == TAG_NULL) {
        _vector_append(&self->dense, value);
        dense->tag = TAG_VALID;
        dense->data = self->dense.elem_count;
    } else {
        _vector_set(&self->dense, dense_index->data, value);
    }
}
void * ss_get(SparseSet * self, uint64_t sparse_index);
void ss_unset(SparseSet * self, uint64_t sparse_index);

int main() {
    vector_run_tests();

    SparseSet s = ss_init(sizeof(SparseSet), 1000);
    ss_deinit(&s);

    return 0;
}
