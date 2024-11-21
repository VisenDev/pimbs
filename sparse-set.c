#include <stdint.h>

#include "testing.h"

#define VECTOR_IMPLEMENTATION
#include "vector.h"
#include "vector.h"

/*
#define SparseSet(T) \
    struct { \
        Vector(T) dense; \
        Vector(uint64_t) dense_to_sparse_map; \
        Vector(uint64_t) sparse; \
    }
    */

int main() {
    vector_run_tests();

    return 0;
}
