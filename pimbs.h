#ifndef PIMBS_H
#define PIMBS_H

#include <stdio.h>
#include "color.h"
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>


//====================TESTING======================
typedef struct {
   const char* name;
   int passed;
   int failed;
} pimbs_testing_Tracker;

typedef struct {
    pimbs_testing_Tracker overall;
    pimbs_testing_Tracker active;
} pimbs_testing_State;

pimbs_testing_State pimbs_testing_start();
void pimbs_testing_end(pimbs_testing_State*);
void pimbs_testing_start_test(pimbs_testing_State *, const char*);
#define pimbs_testing_expect(state, condition, name) _pimbs_testing_expect(state, condition, name, __LINE__, __FILE__)
void pimbs_testing_expect_ext(pimbs_testing_State *, const bool , const char[], const int, const char *);



//===================VECTOR=========================
#define byte char

typedef struct {
    byte* data;
    size_t elem_size_bytes;
    uint64_t elem_count;
    uint64_t elem_capacity;
} pimbs_Vector;

#define pimbs_vector_get(Type, vector, index) (Type*)pimbs_vector_get(vector, index)
void * pimbs_vector_get(const Vector self, const uint64_t index);

#define pimbs_vector_init(Type) _vector_init(sizeof(Type))
Vector _pimbs_vector_init(const size_t elem_size_bytes);
void pimbs_vector_deinit(Vector * self);

#define pimbs_vector_set(vector, index, value) \
    do { \
        typeof(value) value_location = value; \
        _pimbs_vector_set(vector, index, &value_location); \
    } while(0)
void _pimbs_vector_set(Vector * self, uint64_t index, void * value);

#define pimbs_vector_append(vector, value) \
    do { \
        typeof(value) value_location = value; \
        _pimbs_vector_append(vector, &value_location); \
    } while(0)
void _pimbs_vector_append(Vector * self, void * value);
void pimbs_vector_append_array(Vector * self, const void * values, size_t values_count);

#define pimbs_vector_top(Type, vector) (Type*)_vector_top(vector)
void * _pimbs_vector_top(const Vector self);

void pimbs_vector_remove(Vector * self, uint64_t index);
void pimbs_vector_pop(Vector * self);
void pimbs_vector_run_tests();

#endif //PIMBS_H


#ifdef PIMBS_IMPLEMENTATION
#undef PIMBS_IMPLEMENTATION

static void pimbs_testing_print_header(const char * str) {
    const size_t desired_width = 40;
    const size_t len = strlen(str);
    const size_t padding_needed = desired_width - len;
    const size_t pre_padding = padding_needed / 2;
    const size_t post_padding = padding_needed - pre_padding;

    for(uint64_t i = 0; i < pre_padding; ++i) {
        printf("=");
    }
    printf("%s", str);
    for(uint64_t i = 0; i < post_padding; ++i) {
        printf("=");
    }
    printf("\n");
}

static void pimbs_testing_print_summary(const pimbs_testing_Tracker tracker) {
    pimbs_testing_print_header(tracker.name);
    printf("PASSED: " ANSI_COLOR_GREEN "%i\n" ANSI_COLOR_RESET, tracker.passed);
    printf("FAILED: " ANSI_COLOR_RED "%i\n" ANSI_COLOR_RESET, tracker.failed);
    pimbs_testing_print_header("");
}


pimbs_testing_State pimbs_testing_start() {
    return (pimbs_testing_State) {
        .overall = (pimbs_testing_Tracker){.name = "SUMMARY"},
            .active = {0},
    };
}

void pimbs_testing_end(pimbs_testing_State * state) {
    if(state->active.name != NULL) {
        state->overall.passed += state->active.passed;
        state->overall.failed += state->active.failed;
    }
    pimbs_testing_print_summary(state->overall);
    if(state->overall.failed != 0) {
        exit(1);
    } else {
        exit(0);
    }
}


void pimbs_testing_start_test(pimbs_testing_State * state, const char* test_name) {
    if(state->active.name != NULL) {
        state->overall.passed += state->active.passed;
        state->overall.failed += state->active.failed;
    }

    //update active
    state->active = (pimbs_testing_Tracker){0};
    state->active.name = test_name;
}

#define pimbs_testing_expect(state, condition, name) _testing_expect(state, condition, name, __LINE__, __FILE__)
void pimbs_testing_expect_ext(
        pimbs_testing_State * state,
        const bool condition,
        const char name[],
        const int line,
        const char * file
        ) {
    if(condition) {
        printf("./testing %s.%s... " ANSI_COLOR_GREEN "[PASSED]\n" ANSI_COLOR_RESET, state->active.name, name); 
        state->active.passed += 1;
    } else {
        printf("./testing %s.%s... " ANSI_COLOR_RED "[FAILED]\n" ANSI_COLOR_RESET, state->active.name, name); 
        printf(ANSI_COLOR_RED "   -> %s:%i\n" ANSI_COLOR_RESET , file , line); 
        state->active.failed += 1;
    }
}
#endif //PIMBS_IMPLEMENTATION

#define byte char

typedef struct {
    byte* data;
    size_t elem_size_bytes;
    uint64_t elem_count;
    uint64_t elem_capacity;
} pimbs_Vector;

#define pimbs_vector_get(Type, vector, index) (Type*)_vector_get(vector, index)
void * _pimbs_vector_get(const Vector self, const uint64_t index);

#define pimbs_vector_init(Type) _vector_init(sizeof(Type))
Vector _pimbs_vector_init(const size_t elem_size_bytes);
void pimbs_vector_deinit(Vector * self);

#define pimbs_vector_set(vector, index, value) \
    do { \
        typeof(value) value_location = value; \
        _pimbs_vector_set(vector, index, &value_location); \
    } while(0)
void _pimbs_vector_set(Vector * self, uint64_t index, void * value);

#define pimbs_vector_append(vector, value) \
    do { \
        typeof(value) value_location = value; \
        _pimbs_vector_append(vector, &value_location); \
    } while(0)
void _pimbs_vector_append(Vector * self, void * value);
void pimbs_vector_append_array(Vector * self, const void * values, size_t values_count);

#define pimbs_vector_top(Type, vector) (Type*)_vector_top(vector)
void * _pimbs_vector_top(const Vector self);

void pimbs_vector_remove(Vector * self, uint64_t index);
void pimbs_vector_pop(Vector * self);
void pimbs_vector_run_tests();

#endif //VECTOR_H

#ifdef VECTOR_IMPLEMENTATION

static void pimbs_vector_audit(const Vector self)
{
    assert(self.data != NULL);
    assert(self.elem_size_bytes > 0);
    assert(self.elem_count <= self.elem_capacity);
}

static bool pimbs_vector_has_capacity_for(const Vector self, uint64_t item_count)
{
    pimbs_vector_audit(self);
    return (self.elem_count + item_count) <= self.elem_capacity;
}

static void pimbs_vector_double_capacity(Vector * self)
{
    pimbs_vector_audit(*self);
    const uint64_t new_elem_capacity = self->elem_capacity * 2;
    const uint64_t num_bytes_needed = new_elem_capacity * self->elem_size_bytes;
    self->data = realloc(self->data, num_bytes_needed);
    self->elem_capacity = new_elem_capacity;
    pimbs_vector_audit(*self);
}


void * _pimbs_vector_get(const Vector self, const uint64_t index)
{
    pimbs_vector_audit(self);
    assert(index < self.elem_count);
    return self.data + (index * self.elem_size_bytes);
}

void pimbs_vector_deinit(Vector * self)
{
    free(self->data);
    *self = (Vector){0};
}

Vector _pimbs_vector_init(const size_t elem_size_bytes)
{
    const uint64_t initial_capacity = 8;
    const uint64_t capacity_bytes = elem_size_bytes * initial_capacity;

    const Vector result = (Vector){
        .data = malloc(capacity_bytes),
            .elem_size_bytes = elem_size_bytes,
            .elem_capacity = initial_capacity,
            .elem_count = 0,
    };
    pimbs_vector_audit(result);
    return result;
}

void _pimbs_vector_set(Vector * self, uint64_t index, void * value)
{
    pimbs_vector_audit(*self);
    assert(index < self->elem_count);
    memcpy(_pimbs_vector_get(*self, index), value, self->elem_size_bytes);
}

void _pimbs_vector_append(Vector * self, void * value)
{
    pimbs_vector_audit(*self);

    while(!pimbs_vector_has_capacity_for(*self, 1)) {
        pimbs_vector_double_capacity(self);
    }
    self->elem_count += 1;
    _pimbs_vector_set(self, self->elem_count - 1, value);
    pimbs_vector_audit(*self);
}


void pimbs_vector_append_array(Vector * self, const void * values, size_t values_count)
{
    pimbs_vector_audit(*self);

    while(!pimbs_vector_has_capacity_for(*self, values_count)) {
        pimbs_vector_double_capacity(self);
    }
    const uint64_t old_top_index = self->elem_count;
    self->elem_count += values_count;
    memcpy(_pimbs_vector_get(*self, old_top_index), values, (self->elem_size_bytes * values_count));
    //_pimbs_vector_set(self, self->elem_count - 1, value);

    pimbs_vector_audit(*self);
}

void * _pimbs_vector_top(const Vector self) {
   return _pimbs_vector_get(self, self.elem_count - 1);
}

void pimbs_vector_remove(Vector * self, uint64_t index)
{
    pimbs_vector_audit(*self);
    assert(index < self->elem_count);

    void * top = _pimbs_vector_top(*self);
    _pimbs_vector_set(self, index, top);
    self->elem_count -= 1;
    pimbs_vector_audit(*self);
}

void pimbs_vector_pop(Vector * self)
{
    pimbs_vector_remove(self, self->elem_count - 1);
}

void pimbs_vector_run_tests() {
    testing_State t = testing_start();
    testing_start_test(&t, "pimbs_vector-append");
    Vector v = pimbs_vector_init(char);

    pimbs_vector_append(&v, 'a');
    testing_expect(&t, *pimbs_vector_get(char, v, 0) == 'a', "a-initial");

    pimbs_vector_append(&v, 's');
    pimbs_vector_append(&v, 'd');
    pimbs_vector_append(&v, 'f');

    testing_expect(&t, *pimbs_vector_get(char, v, 0) == 'a', "a");
    testing_expect(&t, *pimbs_vector_get(char, v, 1) == 's', "s");
    testing_expect(&t, *pimbs_vector_get(char, v, 2) == 'd', "d");
    testing_expect(&t, *pimbs_vector_get(char, v, 3) == 'f', "f");

    testing_start_test(&t, "pimbs_vector-assign");

    pimbs_vector_set(&v, 0, 'z');
    testing_expect(&t, *pimbs_vector_get(char, v, 0) == 'z', "z-assign");
    testing_expect(&t, *pimbs_vector_get(char, v, 1) == 's', "unchanged-s");
    testing_expect(&t, *pimbs_vector_get(char, v, 2) == 'd', "unchanged-d");
    testing_expect(&t, *pimbs_vector_get(char, v, 3) == 'f', "unchanged-f");

    testing_start_test(&t, "pimbs_vector-top");

    testing_expect(&t, *pimbs_vector_top(char, v) == 'f', "initial");
    pimbs_vector_append(&v, 'g');
    testing_expect(&t, *pimbs_vector_top(char, v) == 'g', "after-append");
    pimbs_vector_pop(&v);
    testing_expect(&t, *pimbs_vector_top(char, v) == 'f', "after-pop");

    testing_start_test(&t, "pimbs_vector-remove");
    const uint64_t old_len = v.elem_count;
    pimbs_vector_remove(&v, 0);
    testing_expect(&t, v.elem_count == (old_len - 1), "elem_count_decrement");

    testing_start_test(&t, "pimbs_vector-append-array");
    pimbs_vector_deinit(&v);
    v = pimbs_vector_init(char);

    const char * str = "hello world";
    const size_t char_count = strlen(str);
    pimbs_vector_append_array(&v, str, char_count);
    pimbs_vector_append(&v, 0);
    testing_expect(&t, strcmp(str, (char *)v.data) == 0, "data-preserved");

    pimbs_vector_deinit(&v);
    testing_end(&t);
}
