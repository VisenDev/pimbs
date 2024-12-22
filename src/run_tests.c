#include <stdbool.h>
#include <stddef.h>
#include "testing.h"
#include "allocator.h"
#include "vector.h"

IMPLEMENT_VECTOR(IntVec, int)

int main() {
    TestingState t = testing_init();
    Allocator a = libc_allocator();

    testing_start_test(&t, "vector");
    IntVec v = IntVec_init(a);
    for(int i = 0; i < 10; ++i) {
       IntVec_append(a, &v, i);
    }

    for(int i = 0; i < 10; ++i) {
        testing_expect(&t, v.items[i] == i);
    }

    testing_deinit(&t);
}

