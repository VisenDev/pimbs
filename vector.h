#ifndef VECTOR_H
#define VECTOR_H

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>

#define TESTING_IMPLEMENTATION
#include "testing.h"

#define byte char

typedef struct {
    byte* data;
    size_t elem_size_bytes;
    uint64_t elem_count;
    uint64_t elem_capacity;
} Vector;

void vector_audit(const Vector self)
#ifdef VECTOR_IMPLEMENTATION
{
    assert(self.data != NULL);
    assert(self.elem_size_bytes > 0);
    assert(self.elem_count <= self.elem_capacity);
}
#else 
;
#endif //VECTOR_IMPLEMENTATION

#define vector_get(Type, vector, index) (Type*)_vector_get(vector, index)
void * _vector_get(const Vector self, const uint64_t index)
#ifdef VECTOR_IMPLEMENTATION
{
    vector_audit(self);
    assert(index < self.elem_count);
    return self.data + (index * self.elem_size_bytes);
}
#else 
;
#endif //VECTOR_IMPLEMENTATION

void vector_deinit(Vector * self)
#ifdef VECTOR_IMPLEMENTATION
{
    free(self->data);
    *self = (Vector){0};
}
#else 
;
#endif //VECTOR_IMPLEMENTATION

#define vector_init(Type) _vector_init(sizeof(Type))
Vector _vector_init(const size_t elem_size_bytes)
#ifdef VECTOR_IMPLEMENTATION
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
#else 
;
#endif //VECTOR_IMPLEMENTATION

bool vector_has_capacity_for(const Vector self, uint64_t item_count)
#ifdef VECTOR_IMPLEMENTATION
{
    vector_audit(self);
    return (self.elem_count + item_count) <= self.elem_capacity;
}
#else 
;
#endif //VECTOR_IMPLEMENTATION

void vector_double_capacity(Vector * self)
#ifdef VECTOR_IMPLEMENTATION
{
    vector_audit(*self);
    const uint64_t new_elem_capacity = self->elem_capacity * 2;
    const uint64_t num_bytes_needed = new_elem_capacity * self->elem_size_bytes;
    self->data = realloc(self->data, num_bytes_needed);
    self->elem_capacity = new_elem_capacity;
    vector_audit(*self);
}
#else 
;
#endif //VECTOR_IMPLEMENTATION


#define vector_set(vector, index, value) \
    do { \
        typeof(value) value_location = value; \
        _vector_set(vector, index, &value_location); \
    } while(0)
void _vector_set(Vector * self, uint64_t index, void * value)
#ifdef VECTOR_IMPLEMENTATION
{
    vector_audit(*self);
    assert(index < self->elem_count);
    memcpy(_vector_get(*self, index), value, self->elem_size_bytes);
}
#else 
;
#endif //VECTOR_IMPLEMENTATION

#define vector_append(vector, value) \
    do { \
        typeof(value) value_location = value; \
        _vector_append(vector, &value_location); \
    } while(0)
void _vector_append(Vector * self, void * value)
#ifdef VECTOR_IMPLEMENTATION
{
    vector_audit(*self);

    while(!vector_has_capacity_for(*self, 1)) {
        vector_double_capacity(self);
    }
    self->elem_count += 1;
    _vector_set(self, self->elem_count - 1, value);
    vector_audit(*self);
}
#else 
;
#endif //VECTOR_IMPLEMENTATION


void vector_append_array(Vector * self, const void * values, size_t values_count)
#ifdef VECTOR_IMPLEMENTATION
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
#else 
;
#endif //VECTOR_IMPLEMENTATION

#define vector_top(Type, vector) (Type*)_vector_top(vector)
void * _vector_top(const Vector self)
#ifdef VECTOR_IMPLEMENTATION
{
   return _vector_get(self, self.elem_count - 1);
}
#else 
;
#endif //VECTOR_IMPLEMENTATION

void vector_remove(Vector * self, uint64_t index)
#ifdef VECTOR_IMPLEMENTATION
{
    vector_audit(*self);
    assert(index < self->elem_count);

    void * top = _vector_top(*self);
    _vector_set(self, index, top);
    self->elem_count -= 1;
    vector_audit(*self);
}
#else 
;
#endif //VECTOR_IMPLEMENTATION

void vector_pop(Vector * self)
#ifdef VECTOR_IMPLEMENTATION
{
    vector_remove(self, self->elem_count - 1);
}
#else 
;
#endif //VECTOR_IMPLEMENTATION

void vector_run_tests()
#ifdef VECTOR_IMPLEMENTATION
{
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
#else 
;
#endif //VECTOR_IMPLEMENTATION
       
#endif //VECTOR_H
