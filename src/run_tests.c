#include <stdbool.h>
#include <stddef.h>
#include "testing.h"
#include "allocator.h"
//#include "vector.h"
//#include "linked_list.h"
//#include "sparse_set.h"

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

    if(false) {
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



//IMPLEMENT_VECTOR(vec, unsigned long)
//IMPLEMENT_LINKED_LIST(list, int)
//IMPLEMENT_SPARSE_SET(set, char)

//int main(void) {
//    TestingState t = testing_init();
//    //Allocator child = libc_allocator();
//    //Allocator a = logging_allocator(&child);
//    Allocator a = libc_allocator();
//
//    //VECTOR
//    {
//        testing_start_test(&t, "vector");
//        vec v = vec_init(a);
//        for(unsigned long i = 0; i < 100000; ++i) {
//            vec_append(a, &v, i);
//        }
//
//        for(unsigned long i = 0; i < 100000; ++i) {
//            if(i % 10000 == 0) {
//                testing_expect(&t, *vec_get(v, i) == i);
//            }
//        }
//        vec_free(a, &v);
//    }
//
//
//    //LINKED LIST
//    {
//        testing_start_test(&t, "linked_list");
//        list * start = NULL;
//        for(int i = 0; i < 100000; ++i) {
//            start = list_cons(a, i, start);
//        }
//
//        list * node = start;
//        for(int i = 99999; i >= 0; --i) {
//            if(i % 10000 == 0) {
//                testing_expect(&t, node->value == i);
//            }
//            node = node->next;
//        }
//        list_free(a, start);
//    }
//
//    //SPARSE SET
//    {
//        testing_start_test(&t, "sparse_set");
//        set s = set_init(a);
//
//        set_set(a, &s, 10, 'a');
//        set_set(a, &s, 20, 'b');
//        set_set(a, &s, 30, 'c');
//        set_set(a, &s, 40, 'd');
//        set_set(a, &s, 50, 'e');
//        testing_expect(&t, *set_get(s, 10) == 'a');
//        testing_expect(&t, *set_get(s, 20) == 'b');
//        testing_expect(&t, *set_get(s, 30) == 'c');
//        testing_expect(&t, *set_get(s, 40) == 'd');
//        testing_expect(&t, *set_get(s, 50) == 'e');
//
//        set_unset(&s, 50);
//        testing_expect(&t, *set_get(s, 10) == 'a');
//        testing_expect(&t, *set_get(s, 20) == 'b');
//        testing_expect(&t, *set_get(s, 30) == 'c');
//        testing_expect(&t, *set_get(s, 40) == 'd');
//        testing_expect(&t, set_get(s, 50) == NULL);
//
//        set_unset(&s, 10);
//        set_set(a, &s, 150, 'f');
//        testing_expect(&t, set_get(s, 10) == NULL);
//        testing_expect(&t, *set_get(s, 20) == 'b');
//        testing_expect(&t, *set_get(s, 30) == 'c');
//        testing_expect(&t, *set_get(s, 40) == 'd');
//        testing_expect(&t, *set_get(s, 150) == 'f');
//
//
//        set_deinit(a, &s);
//    }
//
//    testing_deinit(&t);
//}
//
//
