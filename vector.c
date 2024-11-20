#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

//#define ARENA_IMPLEMENTATION
//#include "arena.h"


//#define Vector(T) \
//    struct { \
//        T* items; \
//        uint64_t len; \
//        uint64_t cap; \
//    }
//


#define byte char

typedef struct {
    byte* data;
    size_t elem_size_bytes;
    uint64_t elem_count;
    uint64_t elem_capacity;
} Vector;


void vector_audit(const Vector self) {
    assert(self.data != NULL);
    assert(self.elem_size_bytes > 0);
    assert(self.elem_count <= self.elem_capacity);
}

void * vector_get(const Vector self, const uint64_t index) {
    vector_audit(self);
    assert(index >= 0);
    assert(index < self.elem_count);
    return self.data + (index * self.elem_size_bytes);
}

Vector vector_init(const size_t elem_size_bytes) {
    const uint64_t initial_capacity = 8;
    const uint64_t capacity_bytes = elem_size_bytes * initial_capacity;

    const Vector result = (Vector){
        .data = malloc(capacity_bytes),
        .elem_size_bytes = elem_size_bytes,
        .elem_capacity = initial_capacity,
        .elem_count = 0,
    };
    vector_audit(result);
    return result;
}

bool vector_has_capacity_for(const Vector self, uint64_t item_count) {
    vector_audit(self);
    return (self.elem_count + item_count) <= self.elem_capacity;
}

void vector_double_capacity(Vector * self) {
    vector_audit(*self);
    const uint64_t new_elem_capacity = self->elem_capacity * 2;
    const uint64_t num_bytes_needed = new_elem_capacity * self->elem_size_bytes;
    self->data = realloc(self->data, num_bytes_needed);
    self->elem_capacity = new_elem_capacity;
    vector_audit(*self);
}

void * vector_append(Vector * self) {
    vector_audit(*self);

    if(!vector_has_capacity_for(*self, 1)) {
        vector_double_capacity(self);
    }
    self->elem_count += 1;
    return vector_get(*self, self->elem_count - 1);
}

int main() {
    return 0;
}
