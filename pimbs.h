#ifndef PIMBS_H
#define PIMBS_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

//=======================ASSERT=================================
#define pimbs_assert(format_specifier, lhs, operator, rhs) \
    do { \
        typeof(lhs) lhs_copy = lhs; \
        typeof(lhs) rhs_copy = rhs; \
        if(!(lhs_copy operator rhs_copy)) { \
            printf( \
                    ANSI_COLOR_RED "\n[Assertion Failure]" ANSI_COLOR_RESET \
                    " assert(" \
                    ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET \
                    " %s "\
                    ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET \
                    ")\n", #lhs, #operator, #rhs \
            ); \
            printf( \
                    "  in" ANSI_COLOR_YELLOW "%s line %d\n" ANSI_COLOR_RESET, \
                    __FILE__, __LINE__); \
            printf( \
                    "  note: \n    " \
                    ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET \
                    " = " \
                    ANSI_COLOR_RED format_specifier ANSI_COLOR_RESET \
                    " \n    " \
                    ANSI_COLOR_YELLOW "%s" ANSI_COLOR_RESET \
                    " = " \
                    ANSI_COLOR_RED format_specifier ANSI_COLOR_RESET \
                    "\n", #lhs, lhs_copy, #rhs, rhs_copy \
            ); \
            abort(); \
        } \
    } while (0)

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

pimbs_testing_State * pimbs_testing_init();
void pimbs_testing_deinit(pimbs_testing_State*);
void pimbs_testing_start_test(pimbs_testing_State *, const char*);
#define pimbs_testing_expect(state, condition) pimbs_testing_expect_internal(state, condition, __LINE__, __FILE__)
void pimbs_testing_expect_internal(pimbs_testing_State *, const bool, const int, const char *);

//===================VECTOR=========================
typedef struct {
    char* data;
    size_t elem_size_bytes;
    uint64_t elem_count;
    uint64_t elem_capacity;
} pimbs_Vector;

//init and deinit
pimbs_Vector * pimbs_vector_init(size_t);
void pimbs_vector_deinit(pimbs_Vector * self);

//get
void * pimbs_vector_get(pimbs_Vector*, uint64_t);
void * pimbs_vector_top(pimbs_Vector * self);

//set and remove
void pimbs_vector_set(pimbs_Vector * self, uint64_t index, void * value);
void pimbs_vector_append(pimbs_Vector * self, void * value);
void pimbs_vector_append_array(pimbs_Vector * self, const void * values, size_t values_count);
void pimbs_vector_remove(pimbs_Vector * self, uint64_t index);
void pimbs_vector_pop(pimbs_Vector * self);
void pimbs_vector_run_tests(pimbs_testing_State*);


//======================SPARSE SET==============================
typedef struct {
    pimbs_Vector * dense;
    pimbs_Vector * sparse;
    pimbs_Vector * dense_to_sparse_map;
} pimbs_SparseSet;

typedef struct {
    enum {PIMBS_TAG_NULL, PIMBS_TAG_VALID} tag;
    uint64_t data;
} pimbs_OptionalIndex;

pimbs_SparseSet * pimbs_ss_init(size_t elem_size_bytes, uint64_t initial_capacity);
void pimbs_ss_deinit(pimbs_SparseSet * self);
void pimbs_ss_run_tests(pimbs_testing_State * t);
void pimbs_ss_unset(pimbs_SparseSet * self, uint64_t sparse_index);
void * pimbs_ss_get(pimbs_SparseSet * self, uint64_t sparse_index);
void pimbs_ss_set(pimbs_SparseSet * self, uint64_t sparse_index, void * value);

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
    fflush(stdout);
}

static void pimbs_testing_print_summary(const pimbs_testing_Tracker tracker) {
    pimbs_testing_print_header(tracker.name);
    printf("PASSED: " ANSI_COLOR_GREEN "%i\n" ANSI_COLOR_RESET, tracker.passed);
    printf("FAILED: " ANSI_COLOR_RED "%i\n" ANSI_COLOR_RESET, tracker.failed);
    pimbs_testing_print_header("");
}


pimbs_testing_State * pimbs_testing_init() {
    pimbs_testing_State * result = malloc(sizeof(pimbs_testing_State));
    *result = (pimbs_testing_State) {
        .overall = (pimbs_testing_Tracker){.name = "SUMMARY"},
            .active = {0},
    };
    return result;
}

void pimbs_testing_deinit(pimbs_testing_State * state) {
    if(state->active.name != NULL) {
        state->overall.passed += state->active.passed;
        state->overall.failed += state->active.failed;
    }
    pimbs_testing_print_summary(state->overall);
    free(state);
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
        const int line,
        const char * file
        ) {
    if(condition) {
        printf("./testing.%s.%d... " ANSI_COLOR_GREEN "[PASSED]\n" ANSI_COLOR_RESET, state->active.name, state->active.passed + state->active.failed); 
        state->active.passed += 1;
    } else {
        printf("./testing.%s.%d... " ANSI_COLOR_RED "[FAILED]\n" ANSI_COLOR_RESET, state->active.name, state->active.passed + state->active.failed); 
        printf(ANSI_COLOR_RED "   -> %s:%i\n" ANSI_COLOR_RESET , file , line); 
        state->active.failed += 1;
    }
}


//===================VECTOR=========================

static void pimbs_vector_audit(pimbs_Vector * self)
{
    pimbs_assert("%s", self->data, !=, NULL);
    pimbs_assert("%zu", self->elem_size_bytes, >, 0);
    pimbs_assert("%llu",self->elem_count, <=, self->elem_capacity);
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


void * pimbs_vector_get(pimbs_Vector * self, const uint64_t index)
{
    pimbs_vector_audit(self);
    pimbs_assert("%llu", index, <, self->elem_count);
    return self->data + (index * self->elem_size_bytes);
}

void pimbs_vector_deinit(pimbs_Vector * self)
{
    free(self->data);
    free(self);
}

pimbs_Vector * pimbs_vector_init(const size_t elem_size_bytes)
{
    const uint64_t initial_capacity = 8;
    const uint64_t capacity_bytes = elem_size_bytes * initial_capacity;

    pimbs_Vector * result = malloc(sizeof(pimbs_Vector));
    *result = (pimbs_Vector){
        .data = malloc(capacity_bytes),
            .elem_size_bytes = elem_size_bytes,
            .elem_capacity = initial_capacity,
            .elem_count = 0,
    };
    pimbs_vector_audit(result);
    return result;
}

void pimbs_vector_set(pimbs_Vector * self, uint64_t index, void * value)
{
    pimbs_vector_audit(self);
    pimbs_assert("%llu", index, <, self->elem_count);
    memcpy(pimbs_vector_get(self, index), value, self->elem_size_bytes);
}

void pimbs_vector_append(pimbs_Vector * self, void * value)
{
    pimbs_vector_audit(self);

    while(!pimbs_vector_has_capacity_for(self, 1)) {
        pimbs_vector_double_capacity(self);
    }
    self->elem_count += 1;
    pimbs_vector_set(self, self->elem_count - 1, value);
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
    memcpy(pimbs_vector_get(self, old_top_index), values, (self->elem_size_bytes * values_count));

    pimbs_vector_audit(self);
}

void * pimbs_vector_top(pimbs_Vector * self) {
   return pimbs_vector_get(self, self->elem_count - 1);
}

void pimbs_vector_remove(pimbs_Vector * self, uint64_t index)
{
    pimbs_vector_audit(self);
    pimbs_assert("%llu", index, <, self->elem_count);

    void * top = pimbs_vector_top(self);
    pimbs_vector_set(self, index, top);
    self->elem_count -= 1;
    pimbs_vector_audit(self);
}

void pimbs_vector_pop(pimbs_Vector * self)
{
    pimbs_vector_remove(self, self->elem_count - 1);
}

void pimbs_vector_run_tests(pimbs_testing_State * t) {
    pimbs_testing_start_test(t, "pimbs.vector.append");
    pimbs_Vector * v = pimbs_vector_init(sizeof(char));

    pimbs_vector_append(v, &(char){'y'});
    pimbs_testing_expect(t, *(char *)pimbs_vector_get(v, 0) == 'y');
    pimbs_vector_set(v, 0, &(char){'a'});

    pimbs_vector_append(v, &(char){'s'});
    pimbs_vector_append(v, &(char){'d'});
    pimbs_vector_append(v, &(char){'f'});

    pimbs_testing_expect(t, *(char *)pimbs_vector_get(v, 0) == 'a');
    pimbs_testing_expect(t, *(char *)pimbs_vector_get(v, 1) == 's');
    pimbs_testing_expect(t, *(char *)pimbs_vector_get(v, 2) == 'd');
    pimbs_testing_expect(t, *(char *)pimbs_vector_get(v, 3) == 'f');

    pimbs_testing_start_test(t, "pimbs.vector.assign");
    pimbs_vector_set(v, 0, &(char){'z'});
    pimbs_testing_expect(t, *(char *)pimbs_vector_get(v, 0) == 'z');
    pimbs_testing_expect(t, *(char *)pimbs_vector_get(v, 1) == 's');
    pimbs_testing_expect(t, *(char *)pimbs_vector_get(v, 2) == 'd');
    pimbs_testing_expect(t, *(char *)pimbs_vector_get(v, 3) == 'f');

    pimbs_testing_start_test(t, "pimbs.vector.top");
    pimbs_testing_expect(t, *(char *)pimbs_vector_top(v) == 'f');
    pimbs_vector_append(v, &(char){'g'});
    pimbs_testing_expect(t, *(char *) pimbs_vector_top(v) == 'g');

    pimbs_testing_start_test(t, "pimbs.vector.pop");
    pimbs_vector_pop(v);
    pimbs_testing_expect(t, *(char *)pimbs_vector_top(v) == 'f');

    pimbs_testing_start_test(t, "pimbs.vector.remove");
    const uint64_t old_len = v->elem_count;
    pimbs_vector_remove(v, 0);
    pimbs_testing_expect(t, v->elem_count == (old_len - 1));

    pimbs_testing_start_test(t, "pimbs.vector.append_array");
    pimbs_vector_deinit(v);
    v = pimbs_vector_init(sizeof(char));

    const char * str = "hello world";
    const size_t char_count = strlen(str);
    pimbs_vector_append_array(v, str, char_count);
    pimbs_vector_append(v, &(char){0});
    pimbs_testing_expect(t, strcmp(str, (char *)v->data) == 0);

    pimbs_vector_deinit(v);
}

//======================SPARSE SET==============================
pimbs_SparseSet * pimbs_ss_init(size_t elem_size_bytes, uint64_t initial_capacity) {
   pimbs_SparseSet * result = malloc(sizeof(pimbs_SparseSet));
   *result = (pimbs_SparseSet){
        .dense = pimbs_vector_init(elem_size_bytes),
        .sparse = pimbs_vector_init(sizeof(pimbs_OptionalIndex)),
        .dense_to_sparse_map = pimbs_vector_init(sizeof(uint64_t)),
   };
   pimbs_OptionalIndex initial_sparse_contents[initial_capacity] = {};
   pimbs_vector_append_array(result->sparse, initial_sparse_contents, initial_capacity);
   return result;
}

void pimbs_ss_deinit(pimbs_SparseSet * self){
    pimbs_vector_deinit(self->dense);
    pimbs_vector_deinit(self->sparse);
    pimbs_vector_deinit(self->dense_to_sparse_map);
    free(self);
}

void pimbs_ss_set(pimbs_SparseSet * self, uint64_t sparse_index, void * value) {
    pimbs_OptionalIndex * dense_index = (pimbs_OptionalIndex *)pimbs_vector_get(self->sparse, sparse_index);
    if(dense_index->tag == PIMBS_TAG_NULL) {
        //printf("index %d not set, setting...\n", sparse_index);
        pimbs_vector_append(self->dense, value);
        dense_index->tag = PIMBS_TAG_VALID;
        dense_index->data = self->dense->elem_count - 1;
        pimbs_vector_append(self->dense_to_sparse_map, &(uint64_t){sparse_index});
    } else {
        //printf("index %d set, overwriting...\n", sparse_index);
        pimbs_vector_set(self->dense, dense_index->data, value);
    }
}

void * pimbs_ss_get(pimbs_SparseSet * self, uint64_t sparse_index) {
    pimbs_OptionalIndex * dense_index = (pimbs_OptionalIndex *)pimbs_vector_get(self->sparse, sparse_index);
    if(dense_index->tag == PIMBS_TAG_NULL) {
        return NULL;
    } else {
        return pimbs_vector_get(self->dense, dense_index->data);
    }
}

void pimbs_ss_unset(pimbs_SparseSet * self, uint64_t sparse_index) {
    pimbs_OptionalIndex * dense_index = (pimbs_OptionalIndex *)pimbs_vector_get(self->sparse, sparse_index);
    if(dense_index->tag == PIMBS_TAG_NULL) {
        return;
    } else if (dense_index->data == self->dense->elem_count - 1) {
       pimbs_vector_pop(self->dense);
       pimbs_vector_pop(self->dense_to_sparse_map);
    } else {
        uint64_t * top_sparse_index = pimbs_vector_top(self->dense_to_sparse_map);
        pimbs_vector_set(self->sparse, *top_sparse_index, dense_index);

        pimbs_vector_remove(self->dense, dense_index->data);
        pimbs_vector_remove(self->dense_to_sparse_map, dense_index->data);
        //pimbs_vector_set(self->sparse, sparse_index, &(pimbs_OptionalIndex){0});
    }
    pimbs_vector_set(self->sparse, sparse_index, &(pimbs_OptionalIndex){0});
}

void pimbs_ss_run_tests(pimbs_testing_State * t) {
    pimbs_testing_start_test(t, "pimbs.sparse_set.init");
    pimbs_SparseSet * ss = pimbs_ss_init(sizeof(char), 1000);

    pimbs_testing_expect(t, ss->dense->elem_count == 0);
    pimbs_testing_expect(t, ss->sparse->elem_count == 1000);
    pimbs_testing_expect(t, ss->dense_to_sparse_map->elem_count == 0);

    pimbs_testing_start_test(t, "pimbs.sparse_set.set");
    pimbs_ss_set(ss, 500, &(char){'a'});
    pimbs_ss_set(ss, 400, &(char){'b'});
    pimbs_ss_set(ss, 300, &(char){'c'});
    pimbs_ss_set(ss, 200, &(char){'d'});
    pimbs_ss_set(ss, 100, &(char){'e'});

    pimbs_testing_start_test(t, "pimbs.sparse_set.get");
    pimbs_testing_expect(t, *(char *)pimbs_ss_get(ss, 500) == 'a');
    pimbs_testing_expect(t, *(char *)pimbs_ss_get(ss, 400) == 'b');
    pimbs_testing_expect(t, *(char *)pimbs_ss_get(ss, 300) == 'c');
    pimbs_testing_expect(t, *(char *)pimbs_ss_get(ss, 200) == 'd');
    pimbs_testing_expect(t, *(char *)pimbs_ss_get(ss, 100) == 'e');

    pimbs_testing_start_test(t, "pimbs.sparse_set.unset");
    pimbs_ss_unset(ss, 500);
    pimbs_testing_expect(t, ss->dense->elem_count == 4);
    pimbs_testing_expect(t, ss->sparse->elem_count == 1000);
    pimbs_testing_expect(t, ss->dense_to_sparse_map->elem_count == 4);
    pimbs_testing_expect(t, *(char *)pimbs_ss_get(ss, 400) == 'b');
    pimbs_testing_expect(t, *(char *)pimbs_ss_get(ss, 300) == 'c');
    pimbs_testing_expect(t, *(char *)pimbs_ss_get(ss, 200) == 'd');
    pimbs_testing_expect(t, *(char *)pimbs_ss_get(ss, 100) == 'e');

    pimbs_ss_unset(ss, 100);
    pimbs_testing_expect(t, ss->dense->elem_count == 3);
    pimbs_testing_expect(t, ss->sparse->elem_count == 1000);
    pimbs_testing_expect(t, ss->dense_to_sparse_map->elem_count == 3);
    pimbs_testing_expect(t, *(char *)pimbs_ss_get(ss, 400) == 'b');
    pimbs_testing_expect(t, *(char *)pimbs_ss_get(ss, 300) == 'c');
    pimbs_testing_expect(t, *(char *)pimbs_ss_get(ss, 200) == 'd');

    pimbs_ss_deinit(ss);
}


//=======================PIMBS GENERIC==================

typedef struct {
    size_t byte_count;
    const char * name;
} pimbs_Type;

bool pimbs_equal_type(pimbs_Type lhs, pimbs_Type rhs) {
    printf("lhs: %s, rhs: %s\n", lhs.name, rhs.name);
    return (lhs.byte_count == rhs.byte_count)
        && (strncmp(lhs.name, rhs.name, 32) == 0);
}

#define PIMBS_TYPE(type) \
   (pimbs_Type){.byte_count = sizeof(type), .name = (const char*)#type}

typedef struct {
    pimbs_Type type;
    char * value;
} pimbs_Generic;

pimbs_Generic * pimbs_generic_init(pimbs_Type type, char * value) {
    pimbs_Generic * result = malloc(sizeof(pimbs_Generic));
    result->value = malloc(type.byte_count); 
    memcpy(result->value, value, type.byte_count);
    result->type = type;
    return result;
}

void pimbs_generic_deinit(pimbs_Generic * value) {
    free(value->value);
    free(value);
}


#define PIMBS_GENERIC(gtype, gvalue) \
    pimbs_generic_init(PIMBS_TYPE(gtype), &(gtype){gvalue})

void * pimbs_generic_extract(pimbs_Type type, pimbs_Generic * value) {
    pimbs_assert("%d", pimbs_equal_type(type, value->type), ==, 1);
    return value->value;
}

#define PIMBS_EXTRACT(type, generic) \
    *(type*)pimbs_generic_extract(PIMBS_TYPE(type), generic)


void pimbs_generic_run_tests(pimbs_testing_State * t) {
    pimbs_testing_start_test(t, "pimbs.generic.basic");
    
    pimbs_Generic * test1 = PIMBS_GENERIC(int, 10);
    pimbs_Generic * test2 = PIMBS_GENERIC(int, 20);
    pimbs_Generic * test3 = PIMBS_GENERIC(int, 30);
    pimbs_Generic * test4 = PIMBS_GENERIC(short, 40);

    if(false) {
        deferred:
        pimbs_generic_deinit(test1);
        pimbs_generic_deinit(test2);
        pimbs_generic_deinit(test3);
        pimbs_generic_deinit(test4);
        return;
    }

    pimbs_testing_expect(t,  pimbs_equal_type(test1->type, test2->type));
    pimbs_testing_expect(t,  pimbs_equal_type(test2->type, test3->type));
    pimbs_testing_expect(t,  pimbs_equal_type(test1->type, test3->type));
    pimbs_testing_expect(t, !pimbs_equal_type(test1->type, test4->type));

    pimbs_testing_start_test(t, "pimbs.generic.extract");
    pimbs_testing_expect(t, PIMBS_EXTRACT(int, test1) == 10);
    pimbs_testing_expect(t, PIMBS_EXTRACT(int, test2) == 20);
    pimbs_testing_expect(t, PIMBS_EXTRACT(int, test3) == 30);
    pimbs_testing_expect(t, PIMBS_EXTRACT(short, test4) == 40);

    goto deferred;
}



//=======================PIMBS LINKED LIST==================

typedef struct pimbs_LinkedList {
    pimbs_Generic * value;
    struct pimbs_LinkedList * next;
    struct pimbs_LinkedList * prev;
} pimbs_LinkedListNode;

pimbs_LinkedListNode * pimbs_llnode_insert
(pimbs_Generic * value, pimbs_LinkedListNode * next, pimbs_LinkedListNode * prev) {
    pimbs_LinkedListNode * result = malloc(sizeof(pimbs_LinkedListNode)); 
    result->value = value;
    result->next = next;
    result->prev = prev;

    if(next) {
        next->prev = result;
    }

    if(prev) {
        prev->next= result;
    }
    return result;
}

//recursively frees all nodes
void pimbs_ll_free(pimbs_LinkedListNode * node) {
    if(node->value) {
        pimbs_generic_deinit(node->value);
        node->value = NULL;
    }

    if(node->next) {
        node->next->prev= NULL;
        pimbs_ll_free(node->next);
    }

    if(node->prev) {
        node->prev->next = NULL;
        pimbs_ll_free(node->prev);
    }

    free(node);
}


void pimbs_ll_run_tests(pimbs_testing_State * t) {
    pimbs_testing_start_test(t, "pimbs.linked_list");
    
    pimbs_Generic * data1 = PIMBS_GENERIC(int, 10);
    pimbs_Generic * data2 = PIMBS_GENERIC(int, 20);
    pimbs_Generic * data3 = PIMBS_GENERIC(int, 30);
    pimbs_Generic * data4 = PIMBS_GENERIC(int, 40);
    pimbs_Generic * data5 = PIMBS_GENERIC(int, 50);

    pimbs_LinkedListNode * node = pimbs_llnode_insert(data1, NULL, NULL);
    pimbs_LinkedListNode * start = node;

    if(false) {
        deferred:
        pimbs_ll_free(start);
        return;
    }

    node = pimbs_llnode_insert(data2, NULL, node);
    node = pimbs_llnode_insert(data3, NULL, node);
    node = pimbs_llnode_insert(data4, NULL, node);
    node = pimbs_llnode_insert(data5, NULL, node);

    pimbs_testing_expect(t, PIMBS_EXTRACT(int, start->value) == 10);
    pimbs_testing_expect(t, PIMBS_EXTRACT(int, start->next->value) == 20);
    pimbs_testing_expect(t, PIMBS_EXTRACT(int, start->next->next->value) == 30);
    pimbs_testing_expect(t, PIMBS_EXTRACT(int, start->next->next->next->value) == 40);
    pimbs_testing_expect(t, PIMBS_EXTRACT(int, start->next->next->next->next->value) == 50);

    goto deferred;
}

//=======================PIMBS HASHSET======================

static uint64_t pimbs_djbt2_hash(const unsigned char *str, const size_t maxlen) {
    //Inspired by djbt2 by Dan Bernstein - http://www.cse.yorku.ca/~oz/hash.html
    uint64_t hash = 5381;
    uint64_t c = 0;

    for(size_t i = 0; (i < maxlen) && (str[i] != 0); ++i) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}


#define MAX_KEY_LEN 16
typedef struct {
    char str[MAX_KEY_LEN]; 
    size_t len;
} pimbs_HashKey;

pimbs_HashKey pimbs_hashkey_init(const char const * str) {
    pimbs_HashKey result = {0};
    result.len = strnlen(str, MAX_KEY_LEN);
    strncpy((char*)&result.str, str, result.len);
    return result;
}

bool pimbs_hashkey_equal(pimbs_HashKey * const lhs, pimbs_HashKey * const rhs) {
    return (lhs->len == rhs->len)
        && (strncmp((char const *)&lhs->str, (char const *)&rhs->str, MAX_KEY_LEN) == 0);
}


static uint64_t pimbs_hash(pimbs_HashKey key) {
    return pimbs_djbt2_hash((char *)&key.str, key.len);
}

typedef struct {
    pimbs_Vector * values;
    pimbs_Vector * keys;
    pimbs_SparseSet * value_indexes;
    uint64_t modulus;
} pimbs_HashSet;

pimbs_HashSet * pimbs_hashset_init(const size_t elem_size_bytes) {
    pimbs_HashSet * result = malloc(sizeof(pimbs_HashSet));
    result->values = pimbs_vector_init(elem_size_bytes); 
    result->keys = pimbs_vector_init(sizeof(pimbs_HashKey));
    result->modulus = 256;
    result->value_indexes = pimbs_ss_init(sizeof(pimbs_LinkedListNode *), result->modulus);
    return result;
}

void pimbs_hashset_put(const pimbs_HashSet * set, char * const key, void * value) {

    //HASHING THE KEY
    const size_t keylen = strnlen(key, MAX_KEY_LEN + 1);
    pimbs_assert("%z", keylen, <=, MAX_KEY_LEN);
    const pimbs_HashKey hashkey = pimbs_hashkey_init(key);

    printf("Inserting key %s\n", hashkey.str);

    const uint64_t hash = pimbs_hash(hashkey);
    uint64_t index = hash % set->modulus;

    printf("Calculating hash: %llu\n", hash);
    printf("Hash modulus %llu: %llu\n", set->modulus, index);
     
    //Find where the value will go and set it
    const uint64_t values_array_index = set->values->elem_count;
    printf("Appending new value to: %d\n", values_array_index);

    pimbs_vector_append(set->values, value);
    pimbs_vector_append(set->keys, &hashkey);

    //printf("Appending new value to: %d\n", values_array_index);

    //Set up the linked list node
    pimbs_LinkedListNode * next = pimbs_ss_get(set->value_indexes, index);
    printf("Current node at values_index[%llu]: %p\n", index, next);

    const pimbs_Generic * node_value = PIMBS_GENERIC(uint64_t, values_array_index);
    printf("New node value: %llu\n", PIMBS_EXTRACT(uint64_t, node_value));

    const pimbs_LinkedListNode * node = pimbs_llnode_insert(node_value, next, NULL);


    //Inset a new value into the sparse set if node exists
    if(next == NULL) {
        pimbs_ss_set(set->value_indexes, index, node);
    }
}

void * pimbs_hashset_get(const pimbs_HashSet * set, char * const key) {

    const size_t keylen = strnlen(key, MAX_KEY_LEN + 1);
    pimbs_assert("%z", keylen, <=, MAX_KEY_LEN);
    const pimbs_HashKey hashkey = pimbs_hashkey_init(key);

    printf("LINE: %d\n", __LINE__);

    const uint64_t hash = pimbs_hash(hashkey);
    uint64_t index = hash % set->modulus;


    printf("LINE: %d\n", __LINE__);

    pimbs_LinkedListNode * node = pimbs_ss_get(set->value_indexes, index);
    for(;node != NULL && node->next != NULL; node = node->next) {

        printf("LINE: %d\n", __LINE__);
        //const pimbs_Generic * node_value = node->value;
        const uint64_t node_index = PIMBS_EXTRACT(uint64_t, node->value);

        printf("LINE: %d\n", __LINE__);
        const pimbs_HashKey * node_index_hashkey = pimbs_vector_get(set->keys, node_index);

        printf("LINE: %d\n", __LINE__);
        if(pimbs_hashkey_equal(&hashkey, node_index_hashkey)) {

        printf("LINE: %d\n", __LINE__);
            return pimbs_vector_get(set->values, node_index);
        } 

        printf("LINE: %d\n", __LINE__);
    }

    return NULL;
    
}


void pimbs_hashset_run_tests(pimbs_testing_State * t) {
    pimbs_testing_start_test(t, "pimbs.hashset.init");
    pimbs_HashSet * h = pimbs_hashset_init(sizeof(int));

    if(false) {
        deferred:
        /* cleanup here */
        return;
    }

    pimbs_testing_start_test(t, "pimbs.hashset.put");
    pimbs_hashset_put(h, "hello", &(int){10});
    
    pimbs_testing_start_test(t, "pimbs.hashset.get");
    pimbs_testing_expect(t, *(int*)pimbs_hashset_get(h, "hello") == 10);

    goto deferred;
}

//=======================PIMBS ECS======================

//typedef struct {
//
//}




//=======================PIMBS JSON=========================
typedef struct {
    pimbs_Vector * bytes;
    pimbs_Vector * status_stack;
    bool prepend_comma;
    enum {PIMBS_JSON_ARRAY, PIMBS_JSON_OBJECT} status;
} pimbs_Json;

void pimbs_json_insert_key(pimbs_Json * self, const char * key) {
    pimbs_assert("%i", self->status, ==, PIMBS_JSON_OBJECT); 
    if(self->prepend_comma) {
        pimbs_vector_append_array(self->bytes, ",\n", 2);
        self->prepend_comma = false;
    }
    pimbs_vector_append_array(self->bytes, "\"", 1);
    pimbs_vector_append_array(self->bytes, key, strlen(key));
    pimbs_vector_append_array(self->bytes, "\": ", 3);
}

void pimbs_json_insert_value_string(pimbs_Json * self, const char * string) {
    self->prepend_comma = true;

    pimbs_vector_append_array(self->bytes, "\"", 1);
    pimbs_vector_append_array(self->bytes, string, strlen(string));
    pimbs_vector_append_array(self->bytes, "\"\n", 2);
}

void pimbs_json_enter_object(pimbs_Json * self) {
    pimbs_vector_append_array(self->bytes, "{", 1);
   //pimbs_vector_append(self->bytes, '"');
   //pimbs_vector_append_array(self->bytes, scope_name, strlen(scope_name));
   //pimbs_vector_append(self->bytes, '"');
   //pimbs_vector_append(self->bytes, ':');
}

void pimbs_json_exit_object(pimbs_Json * self) {
   pimbs_vector_append_array(self->bytes, "}", 1);
}


#endif //PIMBS_IMPLEMENTATION
