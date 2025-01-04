/*#include <stdio.h>
#define LOG_FUNCTION printf*/

#define TUI_IMPLEMENTATION
#include "src/tui.h"

#define TESTING_IMPLEMENTATION
#include "src/testing.h"

/*#define USE_CSTDLIB*/
#define ALLOCATOR_IMPLEMENTATION
#include "src/allocator.h"

#define VEC_IMPLEMENTATION
#define VEC_NAME vec
#define VEC_TYPE int
#include "src/vec.h"

#define VEC_IMPLEMENTATION
#define VEC_NAME str 
#define VEC_TYPE char
#include "src/vec.h"

#define LIST_IMPLEMENTATION
#define LIST_NAME list
#define LIST_TYPE int 
#include "src/list.h"

#define SSET_IMPLEMENTATION
#define SSET_NAME sset
#define SSET_TYPE int
#include "src/sset.h"

#define HASH_IMPLEMENTATION
#define HASH_NAME hashmap
#define HASH_TYPE long
#include "src/hash.h"


/*
#define KVECS_TYPE double
#define KVECS_NAME ecs
#define KVECS_IMPLEMENTATION
#include "src/kvecs.h"
*/

#define KVECS_IMPLEMENTATION
#include "src/json-ecs.h"

#define FORMAT_IMPLEMENTATION
#include "src/format.h"

#include "src/defer.h"


int main(void) {
    TestingState t = testing_init();

    
    #define buflen 0xFFFFFFF
    static char buf[buflen];
    Allocator fixed = fixed_buffer_allocator(buf, buflen);
    /*Allocator libc = libc_allocator();*/
    /*Allocator logging = logging_allocator(&fixed);*/
    Allocator a = leak_check_allocator(&fixed);
    static char fmtbuf[buflen];

    defer(cleanup)
    {

        if(leak_check_count_leaks(a) != 0) {
            tui_printf1("leak count: %d\n", leak_check_count_leaks(a));
        }
        testing_deinit(&t);
        leak_check_allocator_free(a);
    }

    testing_start_test(&t, "kvecs");
    /*{
        ecs e = ecs_init();
        int err = ERR_NONE;
        unsigned int i = 0;


        for(i = 0; i < 100; ++i) {
            const Id entity = ecs_new_entity(a, &e);
            const Id phys = ecs_set_child(a, &e, entity, "physics");
            const Id vel = ecs_set_child(a, &e, phys, "velocity");
            const Id acc = ecs_set_child(a, &e, phys, "acceleration");

            testing_expect(&t, entity.index != NULL_ID);

            err = ecs_set(a, &e, vel, "x", 1.0);
            testing_expect(&t, err == ERR_NONE);
            err = ecs_set(a, &e, vel, "y", 1.0);
            testing_expect(&t, err == ERR_NONE);

            err = ecs_set(a, &e, acc, "x", 0.1);
            testing_expect(&t, err == ERR_NONE);
            err = ecs_set(a, &e, acc, "y", 0.1);
            testing_expect(&t, err == ERR_NONE);

            err = ecs_set(a, &e, phys, "friction", 0.01);
            testing_expect(&t, err == ERR_NONE);
        }

        ecs_free(a, &e);
    }*/


    if(leak_check_count_leaks(a) != 0) {
        tui_printf1("post kvecs leak count: %d\n", leak_check_count_leaks(a));
        leak_check_print_leaks(a);
    }


    testing_start_test(&t, "format_unsigned_long");
    {
        {
            const unsigned long len = format_unsigned_long(fmtbuf, buflen, 1234567890123456789, 10);
            testing_expect(&t, string_equal(fmtbuf, "1234567890123456789", len));
        }
        {
            const unsigned long len = format_unsigned_long(fmtbuf, buflen, 0xDEADBEEF, 16);
            testing_expect(&t, string_equal(fmtbuf, "DEADBEEF", len));
        }
    }


    testing_start_test(&t, "format_double");
    {
        {
            const unsigned long len = format_double(fmtbuf, buflen, 12345.67890, 5);
            testing_expect(&t, string_equal(fmtbuf, "12345.67890", len));
        }
        {
            const unsigned long len = format_double(fmtbuf, buflen, 12345.67890, 2);
            testing_expect(&t, string_equal(fmtbuf, "12345.67", len));
        }
        {
            const unsigned long len = format_double(fmtbuf, buflen, 12345.67890, 0);
            testing_expect(&t, string_equal(fmtbuf, "12345", len));
        }
        {
            const unsigned long len = format_double(fmtbuf, buflen, -12345.67890, 6);
            testing_expect(&t, string_equal(fmtbuf, "-12345.678900", len));
        }
    }


    if(leak_check_count_leaks(a) != 0) {
        tui_printf1("post fmt leak count: %d\n", leak_check_count_leaks(a));
    }


    testing_start_test(&t, "memory_copy");
    {
        unsigned int i = 0;
        unsigned int errors = 0;
        char dest[10000];
        char src[10000];

        for(i = 0; i < 10000; ++i) {
            char ch = (char)(i%255);
            src[i] = ch;
        }
        memory_copy(&dest, &src, 10000);
        for(i = 0; i < 10000; ++i) {
            if(dest[i] != src[i]) {
                errors += 1;
            }
        }

        testing_expect(&t, errors == 0);
    }

    testing_start_test(&t, "vec");
    {
        vec v = vec_init(); 
        int i = 0;

        for(i = 0; i < 10000; ++i){
            const int err = vec_append(a, &v, i);
            simple_assert(err == ERR_NONE, error_name(err));
        }
        for(i = 0; i < 10000; ++i){
            testing_expect(&t, v.items[i] == i);
        }
        vec_free(a, &v);
    }


    if(leak_check_count_leaks(a) != 0) {
        tui_printf1("vec tests leak count: %d\n", leak_check_count_leaks(a));
    }


    testing_start_test(&t, "list");
    {
        list * node = list_cons(a, 0, NULL);
        list * start = node;
        int i = 1;
        simple_assert(node != NULL, "Node allocation failed");

        for(i = 1; i < 10000; ++i){
            node = list_cons(a, i, node);
            simple_assert(node != NULL, "Node allocation failed");
        }
        start = node;
        for(i = 9999; i >= 0; --i){
            testing_expect(&t, node->value == i);
            node = node->next;
        }

        list_free(a, start);
    }


    if(leak_check_count_leaks(a) != 0) {
        tui_printf1("list tests leak count: %d\n", leak_check_count_leaks(a));
    }


    testing_start_test(&t, "sset");
    {
        sset s = sset_init();
        unsigned long i = 0;

        /*set*/
        testing_start_test(&t, "sset.put");
        for(i = 0; i < 10000; i += 1){
            const int err = sset_put(a, &s, i * 2, (int)(i / 10));
            simple_assert(err == ERR_NONE, error_name(err));
        }

        testing_start_test(&t, "sset.get");
        for(i = 0; i < 10000; i += 1){
            testing_expect(&t, *sset_get(&s, i * 2) == (int)(i / 10));
        }

        /*unset*/
        testing_start_test(&t, "sset.delete");
        for(i = 0; i < 10000; i += 1){
            if(i % 333 == 0) {
                const int err = sset_delete(&s, i * 2);
                simple_assert(err == ERR_NONE, error_name(err));
            }
        }

        for(i = 0; i < 10000; i += 1){
            if(i % 333 == 0) {
                testing_expect(&t, sset_get(&s, i * 2) == NULL);
            } else {
                testing_expect(&t, *sset_get(&s, i * 2) == (int)(i / 10));
            }
        }


        testing_start_test(&t, "sset.get_or_set");
        for(i = 30000; i < 40000; i += 1){
            const int * value = sset_get_or_put(a, &s, i, (int)i);
            testing_expect(&t, SAFE_DEREF(value) == (int)i);
        }

        testing_start_test(&t, "sset.get_or_set_no_mangle");
        for(i = 30000; i < 40000; i += 1){
            testing_expect(&t, *sset_get(&s, i) == (int)i);
        }

        sset_free(a, &s);
    }


    if(leak_check_count_leaks(a) != 0) {
        tui_printf1("sset tests leak count: %d\n", leak_check_count_leaks(a));
    }


    testing_start_test(&t, "hashmap");
    {
        int err = 0;
        hashmap h = hashmap_init();

        err = hashmap_put(a, &h, "asdf", 1);
        simple_assert(err == ERR_NONE, error_name(err));

        err = hashmap_put(a, &h, "ASDF", 2);
        simple_assert(err == ERR_NONE, error_name(err));

        err = hashmap_put(a, &h, "IDGAF", 3);
        simple_assert(err == ERR_NONE, error_name(err));

        testing_expect(&t, *hashmap_get(&h, "asdf") == 1);
        testing_expect(&t, *hashmap_get(&h, "ASDF") == 2);
        testing_expect(&t, *hashmap_get(&h, "IDGAF") == 3);
        hashmap_free(a, &h);
    }


    deferred(cleanup);
    return 0;
}

