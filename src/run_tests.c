#include <stddef.h>
#include <stdio.h>
#define LOG_FUNCTION printf

#define TESTING_IMPLEMENTATION
#include "testing.h"
#define ALLOCATOR_IMPLEMENTATION
#include "allocator.h"
#include "assert.h"

#define VEC_IMPLEMENTATION
#define VEC_NAME vec
#define VEC_TYPE int
#include "vec.h"

#define VEC_IMPLEMENTATION
#define VEC_NAME str 
#define VEC_TYPE char
#include "vec.h"

#define LIST_IMPLEMENTATION
#define LIST_NAME list
#define LIST_TYPE int 
#include "list.h"

#define SSET_IMPLEMENTATION
#define SSET_NAME sset
#define SSET_TYPE int
#include "sset.h"


int main(void) {
    TestingState t = testing_init();
    Allocator libc = libc_allocator();
    //Allocator child = logging_allocator(&libc);
    Allocator a = leak_check_allocator(libc);
    //Allocator a = logging_allocator(&child);
    //Allocator a = tsoding_arena_allocator();
    
    if(0) {
        cleanup:

        if(leak_check_count_leaks(a) != 0) {
            debug_printf("leak count: %d\n", leak_check_count_leaks(a));
        }
        testing_deinit(&t);
        leak_check_allocator_free(a);
        return 0;
    }

    testing_start_test(&t, "vec");
    {
        vec v = vec_init(); 

        for(int i = 0; i < 10000; ++i){
            vec_append(a, &v, i);
        }
        for(int i = 0; i < 10000; ++i){
            if(i % 1000 == 0) {
                testing_expect(&t, v.items[i] == i);
            }
        }
        vec_free(a, &v);
    }


    if(leak_check_count_leaks(a) != 0) {
        debug_printf("vec tests leak count: %d\n", leak_check_count_leaks(a));
    }


    testing_start_test(&t, "list");
    {
        list * node = NULL;

        for(int i = 0; i < 10000; ++i){
            node = list_cons(a, i, node);
        }
        list * start = node;
        for(int i = 9999; i >= 0; --i){
            if(i % 1000 == 0) {
                testing_expect(&t, node->value == i);
            }
            node = node->next;
        }

        list_free(a, start);
    }


    if(leak_check_count_leaks(a) != 0) {
        debug_printf("list tests leak count: %d\n", leak_check_count_leaks(a));
    }


    testing_start_test(&t, "sset");
    {
        sset s = sset_init();

        //set
        testing_start_test(&t, "sset.put");
        for(unsigned long i = 0; i < 10000; i += 1){
            const int err = sset_put(a, &s, i * 2, (int)(i / 10));
            assert(!err);
        }

        testing_start_test(&t, "sset.get");
        for(unsigned long i = 0; i < 10000; i += 1){
            if(i % 1000 == 0) {
                testing_expect(&t, *sset_get(&s, i * 2) == (int)(i / 10));
            }
        }

        //unset
        testing_start_test(&t, "sset.delete");
        for(unsigned long i = 0; i < 10000; i += 1){
            if(i % 333 == 0) {
                const int err = sset_delete(&s, i * 2);
                assert(!err);
            }
        }

        testing_start_test(&t, "sset.test_deleted");
        for(unsigned long i = 0; i < 10000; i += 1){
            if(i % 333 == 0) {
                testing_expect(&t, sset_get(&s, i * 2) == NULL);
            } else if(i % 1000 == 0) {
                testing_expect(&t, *sset_get(&s, i * 2) == (int)(i / 10));
            }
        }

        sset_free(a, &s);
    }


    if(leak_check_count_leaks(a) != 0) {
        debug_printf("sset tests leak count: %d\n", leak_check_count_leaks(a));
    }

    goto cleanup;
}
