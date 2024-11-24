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

pimbs_testing_State * pimbs_testing_start();
void pimbs_testing_end(pimbs_testing_State*);
void pimbs_testing_start_test(pimbs_testing_State *, const char*);
#define pimbs_testing_expect(state, condition, name) pimbs_testing_expect_internal(state, condition, name, __LINE__, __FILE__)
void pimbs_testing_expect_internal(pimbs_testing_State *, const bool , const char[], const int, const char *);

//===================VECTOR=========================
#define byte char

typedef struct {
    byte* data;
    size_t elem_size_bytes;
    uint64_t elem_count;
    uint64_t elem_capacity;
} pimbs_Vector;

#define pimbs_vector_get(Type, vector, index) (Type*)pimbs_vector_get_internal(vector, index)
void * pimbs_vector_get_internal(pimbs_Vector*, uint64_t);

#define pimbs_vector_init(Type) pimbs_vector_init_internal(sizeof(Type))
pimbs_Vector pimbs_vector_init_internal(size_t);
void pimbs_vector_deinit(pimbs_Vector * self);

#define pimbs_vector_set(vector, index, value) \
    do { \
        typeof(value) value_location = value; \
        pimbs_vector_set_internal(vector, index, &value_location); \
    } while(0)
void pimbs_vector_set_internal(pimbs_Vector * self, uint64_t index, void * value);

#define pimbs_vector_append(vector, value) \
    do { \
        typeof(value) value_location = value; \
        pimbs_vector_append_internal(vector, &value_location); \
    } while(0)
void pimbs_vector_append_internal(pimbs_Vector * self, void * value);
void pimbs_vector_append_array(pimbs_Vector * self, const void * values, size_t values_count);

#define pimbs_vector_top(Type, vector) (Type*)pimbs_vector_top_internal(vector)
void * pimbs_vector_top_internal(pimbs_Vector * self);

void pimbs_vector_remove(pimbs_Vector * self, uint64_t index);
void pimbs_vector_pop(pimbs_Vector * self);
void pimbs_vector_run_tests(pimbs_testing_State*);

#endif //PIMBS_H


#ifdef PIMBS_IMPLEMENTATION
#undef PIMBS_IMPLEMENTATION

//====================TESTING======================

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


pimbs_testing_State * pimbs_testing_start() {
    pimbs_testing_State * result = malloc(sizeof(pimbs_testing_State));
    *result = (pimbs_testing_State) {
        .overall = (pimbs_testing_Tracker){.name = "SUMMARY"},
            .active = {0},
    };
    return result;
}

void pimbs_testing_end(pimbs_testing_State * state) {
    if(state->active.name != NULL) {
        state->overall.passed += state->active.passed;
        state->overall.failed += state->active.failed;
    }
    pimbs_testing_print_summary(state->overall);
    free(state);
    //if(state->overall.failed != 0) {
    //    exit(1);
    //} else {
    //    exit(0);
    //}
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

void pimbs_testing_expect_internal(
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


//===================VECTOR=========================
//
static void pimbs_vector_audit(pimbs_Vector * self)
{
    assert(self->data != NULL);
    assert(self->elem_size_bytes > 0);
    assert(self->elem_count <= self->elem_capacity);
}

static bool pimbs_vector_has_capacity_for(pimbs_Vector * self, uint64_t item_count)
{
    pimbs_vector_audit(self);
    return (self->elem_count + item_count) <= self->elem_capacity;
}

static void pimbs_vector_double_capacity(pimbs_Vector * self)
{
    pimbs_vector_audit(self);
    const uint64_t new_elem_capacity = self->elem_capacity * 2;
    const uint64_t num_bytes_needed = new_elem_capacity * self->elem_size_bytes;
    self->data = realloc(self->data, num_bytes_needed);
    self->elem_capacity = new_elem_capacity;
    pimbs_vector_audit(self);
}


void * pimbs_vector_get_internal(pimbs_Vector * self, const uint64_t index)
{
    pimbs_vector_audit(self);
    assert(index < self->elem_count);
    return self->data + (index * self->elem_size_bytes);
}

void pimbs_vector_deinit(pimbs_Vector * self)
{
    free(self->data);
    *self = (pimbs_Vector){0};
}

pimbs_Vector pimbs_vector_init_internal(const size_t elem_size_bytes)
{
    const uint64_t initial_capacity = 8;
    const uint64_t capacity_bytes = elem_size_bytes * initial_capacity;

    pimbs_Vector result = (pimbs_Vector){
        .data = malloc(capacity_bytes),
            .elem_size_bytes = elem_size_bytes,
            .elem_capacity = initial_capacity,
            .elem_count = 0,
    };
    pimbs_vector_audit(&result);
    return result;
}

void pimbs_vector_set_internal(pimbs_Vector * self, uint64_t index, void * value)
{
    pimbs_vector_audit(self);
    assert(index < self->elem_count);
    memcpy(pimbs_vector_get_internal(self, index), value, self->elem_size_bytes);
}

void pimbs_vector_append_internal(pimbs_Vector * self, void * value)
{
    pimbs_vector_audit(self);

    while(!pimbs_vector_has_capacity_for(self, 1)) {
        pimbs_vector_double_capacity(self);
    }
    self->elem_count += 1;
    pimbs_vector_set_internal(self, self->elem_count - 1, value);
    pimbs_vector_audit(self);
}


void pimbs_vector_append_array(pimbs_Vector * self, const void * values, size_t values_count)
{
    pimbs_vector_audit(self);

    while(!pimbs_vector_has_capacity_for(self, values_count)) {
        pimbs_vector_double_capacity(self);
    }
    const uint64_t old_top_index = self->elem_count;
    self->elem_count += values_count;
    memcpy(pimbs_vector_get_internal(self, old_top_index), values, (self->elem_size_bytes * values_count));
    //_pimbs_vector_set(self, self->elem_count - 1, value);

    pimbs_vector_audit(self);
}

void * pimbs_vector_top_internal(pimbs_Vector * self) {
   return pimbs_vector_get_internal(self, self->elem_count - 1);
}

void pimbs_vector_remove(pimbs_Vector * self, uint64_t index)
{
    pimbs_vector_audit(self);
    assert(index < self->elem_count);

    void * top = pimbs_vector_top_internal(self);
    pimbs_vector_set_internal(self, index, top);
    self->elem_count -= 1;
    pimbs_vector_audit(self);
}

void pimbs_vector_pop(pimbs_Vector * self)
{
    pimbs_vector_remove(self, self->elem_count - 1);
}

void pimbs_vector_run_tests(pimbs_testing_State * t) {
    pimbs_testing_start_test(t, "pimbs_vector-append");
    pimbs_Vector v = pimbs_vector_init(char);

    pimbs_vector_append(&v, 'y');
    pimbs_testing_expect(t, *pimbs_vector_get(char, &v, 0) == 'y', "y-initial");
    pimbs_vector_set_internal(&v, 0, &(char){'a'});

    pimbs_vector_append(&v, 's');
    pimbs_vector_append(&v, 'd');
    pimbs_vector_append(&v, 'f');

    pimbs_testing_expect(t, *pimbs_vector_get(char, &v, 0) == 'a', "a");
    pimbs_testing_expect(t, *pimbs_vector_get(char, &v, 1) == 's', "s");
    pimbs_testing_expect(t, *pimbs_vector_get(char, &v, 2) == 'd', "d");
    pimbs_testing_expect(t, *pimbs_vector_get(char, &v, 3) == 'f', "f");

    pimbs_testing_start_test(t, "pimbs_vector-assign");

    pimbs_vector_set(&v, 0, 'z');
    pimbs_testing_expect(t, *pimbs_vector_get(char, &v, 0) == 'z', "z-assign");
    pimbs_testing_expect(t, *pimbs_vector_get(char, &v, 1) == 's', "unchanged-s");
    pimbs_testing_expect(t, *pimbs_vector_get(char, &v, 2) == 'd', "unchanged-d");
    pimbs_testing_expect(t, *pimbs_vector_get(char, &v, 3) == 'f', "unchanged-f");

    pimbs_testing_start_test(t, "pimbs_vector-top");

    pimbs_testing_expect(t, *pimbs_vector_top(char, &v) == 'f', "initial");
    pimbs_vector_append(&v, 'g');
    pimbs_testing_expect(t, *pimbs_vector_top(char, &v) == 'g', "after-append");
    pimbs_vector_pop(&v);
    pimbs_testing_expect(t, *pimbs_vector_top(char, &v) == 'f', "after-pop");

    pimbs_testing_start_test(t, "pimbs_vector-remove");
    const uint64_t old_len = v.elem_count;
    pimbs_vector_remove(&v, 0);
    pimbs_testing_expect(t, v.elem_count == (old_len - 1), "elem_count_decrement");

    pimbs_testing_start_test(t, "pimbs_vector-append-array");
    pimbs_vector_deinit(&v);
    v = pimbs_vector_init(char);

    const char * str = "hello world";
    const size_t char_count = strlen(str);
    pimbs_vector_append_array(&v, str, char_count);
    pimbs_vector_append(&v, 0);
    pimbs_testing_expect(t, strcmp(str, (char *)v.data) == 0, "data-preserved");

    pimbs_vector_deinit(&v);
}


//======================SPARSE SET===============================
typedef struct {
    pimbs_Vector dense;
    pimbs_Vector sparse;
    pimbs_Vector dense_to_sparse_map;
} pimbs_SparseSet;

typedef struct {
    enum {PIMBS_TAG_NULL, PIMBS_TAG_VALID} tag;
    uint64_t data;
} pimbs_OptionalIndex;

pimbs_SparseSet pimbs_ss_init(size_t elem_size_bytes, uint64_t initial_capacity) {
   pimbs_SparseSet result = (pimbs_SparseSet){
        .dense = pimbs_vector_init_internal(elem_size_bytes),
        .sparse = pimbs_vector_init(pimbs_OptionalIndex),
        .dense_to_sparse_map = pimbs_vector_init(uint64_t),
   };
   pimbs_OptionalIndex initial_sparse_contents[initial_capacity] = {};
   pimbs_vector_append_array(&result.sparse, initial_sparse_contents, initial_capacity);
   return result;
}

void pimbs_ss_deinit(pimbs_SparseSet * self){
    pimbs_vector_deinit(&self->dense);
    pimbs_vector_deinit(&self->sparse);
    pimbs_vector_deinit(&self->dense_to_sparse_map);
    *self = (pimbs_SparseSet){0};
}

void pimbs_ss_set_internal(pimbs_SparseSet * self, uint64_t sparse_index, void * value) {
    pimbs_OptionalIndex * dense_index = pimbs_vector_get(pimbs_OptionalIndex, &self->sparse, sparse_index);
    if(dense_index->tag == PIMBS_TAG_NULL) {
        printf("index %d not set, setting...\n", sparse_index);
        pimbs_vector_append_internal(&self->dense, value);
        dense_index->tag = PIMBS_TAG_VALID;
        dense_index->data = self->dense.elem_count;
        pimbs_vector_append(&self->dense_to_sparse_map, sparse_index);
    } else {
        printf("index %d set, overwriting...\n", sparse_index);
        pimbs_vector_set_internal(&self->dense, dense_index->data, value);
    }
}

void * pimbs_ss_get_internal(pimbs_SparseSet * self, uint64_t sparse_index) {
    pimbs_OptionalIndex * dense_index = pimbs_vector_get(pimbs_OptionalIndex, &self->sparse, sparse_index);
    if(dense_index->tag == PIMBS_TAG_NULL) {
        return NULL;
    } else {
        return pimbs_vector_get_internal(&self->dense, dense_index->data);
    }
}

void pimbs_ss_unset(pimbs_SparseSet * self, uint64_t sparse_index) {
    pimbs_OptionalIndex * dense_index = pimbs_vector_get(pimbs_OptionalIndex, &self->sparse, sparse_index);
    if(dense_index->tag == PIMBS_TAG_NULL) {
        return;
    } else  {
        pimbs_vector_remove(&self->dense, dense_index->data);
        pimbs_vector_remove(&self->dense_to_sparse_map, dense_index->data);
        pimbs_vector_set(&self->sparse, sparse_index, (pimbs_OptionalIndex){0});
    }
}

void pimbs_ss_run_tests(pimbs_testing_State * t) {
    pimbs_testing_start_test(t, "pimbs_sparse_set");
    pimbs_SparseSet ss = pimbs_ss_init(sizeof(char), 1000);

    pimbs_testing_expect(t, ss.dense.elem_count == 0, "dense.correct_size");
    pimbs_testing_expect(t, ss.sparse.elem_count == 1000, "sparse.correct_size");
    pimbs_testing_expect(t, ss.dense_to_sparse_map.elem_count == 0, "dense_to_sparse_map.correct_size");

    pimbs_ss_set_internal(&ss, 500, &(char){'a'});
    pimbs_ss_set_internal(&ss, 400, &(char){'b'});
    pimbs_ss_set_internal(&ss, 300, &(char){'c'});
    pimbs_ss_set_internal(&ss, 200, &(char){'d'});
    pimbs_ss_set_internal(&ss, 100, &(char){'e'});

    pimbs_testing_expect(t, *(char *)pimbs_ss_get_internal(&ss, 500) == 'a', "get_a");
    pimbs_testing_expect(t, *(char *)pimbs_ss_get_internal(&ss, 400) == 'b', "get_b");
    pimbs_testing_expect(t, *(char *)pimbs_ss_get_internal(&ss, 300) == 'c', "get_c");
    pimbs_testing_expect(t, *(char *)pimbs_ss_get_internal(&ss, 200) == 'd', "get_d");
    pimbs_testing_expect(t, *(char *)pimbs_ss_get_internal(&ss, 100) == 'e', "get_e");

    pimbs_ss_deinit(&ss);
}

#endif //PIMBS_IMPLEMENTATION
