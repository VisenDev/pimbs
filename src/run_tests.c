#include <stdbool.h>
#include <stddef.h>
#include "testing.h"
#include "allocator.h"
#include "vector.h"
#include "linked_list.h"

IMPLEMENT_VECTOR(vec, int)
IMPLEMENT_LINKED_LIST(list, int)

int main() {
    TestingState t = testing_init();
    Allocator a = libc_allocator();

    //VECTOR
    {
        testing_start_test(&t, "vector");
        vec v = vec_init(a);
        for(int i = 0; i < 10; ++i) {
            vec_append(a, &v, i);
        }

        for(int i = 0; i < 10; ++i) {
            testing_expect(&t, v.items[i] == i);
        }
        vec_free(a, &v);
    }


    //LINKED LIST
    {
        testing_start_test(&t, "linked_list");
        list * start = NULL;
        for(int i = 0; i < 10; ++i) {
            start = list_cons(a, i, start);
        }

        list * node = start;
        for(int i = 9; i >= 0; --i) {
            testing_expect(&t, node->value == i);
            node = node->next;
        }
        list_free(a, start);
    }

    testing_deinit(&t);
}


