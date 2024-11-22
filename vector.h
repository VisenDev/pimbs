#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include "testing.h"

#define byte char

typedef struct {
    byte* data;
    size_t elem_size_bytes;
    uint64_t elem_count;
    uint64_t elem_capacity;
} Vector;

#define vector_get(Type, vector, index) (Type*)_vector_get(vector, index)
void * _vector_get(const Vector self, const uint64_t index);

#define vector_init(Type) _vector_init(sizeof(Type))
Vector _vector_init(const size_t elem_size_bytes);
void vector_deinit(Vector * self);

#define vector_set(vector, index, value) \
    do { \
        typeof(value) value_location = value; \
        _vector_set(vector, index, &value_location); \
    } while(0)
void _vector_set(Vector * self, uint64_t index, void * value);

#define vector_append(vector, value) \
    do { \
        typeof(value) value_location = value; \
        _vector_append(vector, &value_location); \
    } while(0)
void _vector_append(Vector * self, void * value);
void vector_append_array(Vector * self, const void * values, size_t values_count);

#define vector_top(Type, vector) (Type*)_vector_top(vector)
void * _vector_top(const Vector self);

void vector_remove(Vector * self, uint64_t index);
void vector_pop(Vector * self);
void vector_run_tests();

#endif //VECTOR_H

#ifdef VECTOR_IMPLEMENTATION

static void vector_audit(const Vector self)
{
    assert(self.data != NULL);
    assert(self.elem_size_bytes > 0);
    assert(self.elem_count <= self.elem_capacity);
}

static bool vector_has_capacity_for(const Vector self, uint64_t item_count)
{
    vector_audit(self);
    return (self.elem_count + item_count) <= self.elem_capacity;
}

static void vector_double_capacity(Vector * self)
{
    vector_audit(*self);
    const uint64_t new_elem_capacity = self->elem_capacity * 2;
    const uint64_t num_bytes_needed = new_elem_capacity * self->elem_size_bytes;
    self->data = realloc(self->data, num_bytes_needed);
    self->elem_capacity = new_elem_capacity;
    vector_audit(*self);
}


void * _vector_get(const Vector self, const uint64_t index)
{
    vector_audit(self);
    assert(index < self.elem_count);
    return self.data + (index * self.elem_size_bytes);
}

void vector_deinit(Vector * self)
{
    free(self->data);
    *self = (Vector){0};
}

Vector _vector_init(const size_t elem_size_bytes)
{
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

void _vector_set(Vector * self, uint64_t index, void * value)
{
    vector_audit(*self);
    assert(index < self->elem_count);
    memcpy(_vector_get(*self, index), value, self->elem_size_bytes);
}

void _vector_append(Vector * self, void * value)
{
    vector_audit(*self);

    while(!vector_has_capacity_for(*self, 1)) {
        vector_double_capacity(self);
    }
    self->elem_count += 1;
    _vector_set(self, self->elem_count - 1, value);
    vector_audit(*self);
}


void vector_append_array(Vector * self, const void * values, size_t values_count)
{
    vector_audit(*self);

    while(!vector_has_capacity_for(*self, values_count)) {
        vector_double_capacity(self);
    }
    const uint64_t old_top_index = self->elem_count;
    self->elem_count += values_count;
    memcpy(_vector_get(*self, old_top_index), values, (self->elem_size_bytes * values_count));
    //_vector_set(self, self->elem_count - 1, value);

    vector_audit(*self);
}

void * _vector_top(const Vector self) {
   return _vector_get(self, self.elem_count - 1);
}

void vector_remove(Vector * self, uint64_t index)
{
    vector_audit(*self);
    assert(index < self->elem_count);

    void * top = _vector_top(*self);
    _vector_set(self, index, top);
    self->elem_count -= 1;
    vector_audit(*self);
}

void vector_pop(Vector * self)
{
    vector_remove(self, self->elem_count - 1);
}

void vector_run_tests() {
    testing_State t = testing_start();
    testing_start_test(&t, "vector-append");
    Vector v = vector_init(char);

    vector_append(&v, 'a');
    testing_expect(&t, *vector_get(char, v, 0) == 'a', "a-initial");

    vector_append(&v, 's');
    vector_append(&v, 'd');
    vector_append(&v, 'f');

    testing_expect(&t, *vector_get(char, v, 0) == 'a', "a");
    testing_expect(&t, *vector_get(char, v, 1) == 's', "s");
    testing_expect(&t, *vector_get(char, v, 2) == 'd', "d");
    testing_expect(&t, *vector_get(char, v, 3) == 'f', "f");

    testing_start_test(&t, "vector-assign");

    vector_set(&v, 0, 'z');
    testing_expect(&t, *vector_get(char, v, 0) == 'z', "z-assign");
    testing_expect(&t, *vector_get(char, v, 1) == 's', "unchanged-s");
    testing_expect(&t, *vector_get(char, v, 2) == 'd', "unchanged-d");
    testing_expect(&t, *vector_get(char, v, 3) == 'f', "unchanged-f");

    testing_start_test(&t, "vector-top");

    testing_expect(&t, *vector_top(char, v) == 'f', "initial");
    vector_append(&v, 'g');
    testing_expect(&t, *vector_top(char, v) == 'g', "after-append");
    vector_pop(&v);
    testing_expect(&t, *vector_top(char, v) == 'f', "after-pop");

    testing_start_test(&t, "vector-remove");
    const uint64_t old_len = v.elem_count;
    vector_remove(&v, 0);
    testing_expect(&t, v.elem_count == (old_len - 1), "elem_count_decrement");

    testing_start_test(&t, "vector-append-array");
    vector_deinit(&v);
    v = vector_init(char);

    const char * str = "hello world";
    const size_t char_count = strlen(str);
    vector_append_array(&v, str, char_count);
    vector_append(&v, 0);
    testing_expect(&t, strcmp(str, (char *)v.data) == 0, "data-preserved");

    vector_deinit(&v);
    testing_end(&t);
}
#endif //VECTOR_IMPLEMENTATION
