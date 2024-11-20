#include <stdint.h>


#define SparseSet(T) \
    struct { \
        Vector(T) dense; \
        Vector(uint64_t) dense_to_sparse_map; \
        Vector(uint64_t) sparse; \
    }

int main() {
    SparseSet(char) myset = {};

    return 0;
}
