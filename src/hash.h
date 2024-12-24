#include "allocator.h"
#include "concat.h"
#include "debug.h"

#ifndef HASH_TYPE 
#error "HASH_TYPE must be defined before including \"hash.h\""
#endif //LIST TYPE
       
#ifndef HASH_NAME
#error "HASH_NAME must be defined before including \"hash.h\""
#endif //LIST_NAME

// Subtypes
#ifdef HASH_IMPLEMENTATION
#define VEC_IMPLEMENTATION
#define SSET_IMPLEMENTATION
#endif //HASH_IMPLEMENTATION

#define Values CONCAT(SSET_NAME, _values)
#define VEC_NAME Values
#define VEC_TYPE HASH_TYPE
#include "vec.h"

#define Keys CONCAT(SSET_NAME, _keys)
#define VEC_NAME Keys
#define VEC_TYPE char *
#include "vec.h"

#define IndexRecord CONCAT(SSET_NAME, _index_records)
#define LIST_NAME IndexRecord
#define LIST_TYPE unsigned long
#include "list.h"

#define Indexes CONCAT(SSET_NAME, _indexes)
#define SSET_NAME Indexes
#define SSET_TYPE IndexRecord
#include "sset.h"

#undef VEC_IMPLEMENTATION
#undef SSET_IMPLEMENTATION

typedef struct {
    unsigned long max_key_len;
    unsigned long modulus;
    Values values;
    Keys keys;
    Indexes indexes;
} HASH_NAME;

HASH_NAME CONCAT(HASH_NAME, _init)(void) 
#ifdef SSET_IMPLEMENTATION
{
    return (HASH_NAME){
        .max_key_len = 64,
        .modulus = 1024,
        .values = CONCAT(Values, _init)(),
        .keys = CONCAT(Keys, _init)(),
        .indexes = CONCAT(Indexes, _init)(),
    };
}
#else
;
#endif


HASH_NAME CONCAT(HASH_NAME, _free)(Allocator a, HASH_NAME * self) 
#ifdef SSET_IMPLEMENTATION
{
    for(unsigned long i = 0; i < self->keys.len; ++i) {
        a.free(a, self->keys.items[i]);
    }

    for(unsigned long i = 0; i < self->indexes.len; ++i) {
        CONCAT(IndexRecord, _free)(a, self->indexes.items[i];
    }

    CONCAT(Values, _free)(a, &self->values);
    CONCAT(Keys, _free)(a, &self->keys);
    CONCAT(Indexes, _free)(a, &self->indexes);
}
#else
;
#endif

unsigned long CONCAT(HASH_NAME, _hash)(HASH_NAME * const self, const char * key) 
#ifdef SSET_IMPLEMENTATION
{
    /*Inspired by djbt2 by Dan Bernstein - http://www.cse.yorku.ca/~oz/hash.html */
    unsigned long hash = 5381;
    unsigned long c = 0;

    for(unsigned long i = 0; (i < self->max_key_len) && (key[i] != 0); ++i) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash % self->modulus;
}
#else
;
#endif

int CONCAT(HASH_NAME, _put)(Allocator a, HASH_NAME * self, const char * key, const HASH_TYPE value)
#ifdef SSET_IMPLEMENTATION
{
    const char * keycpy = string_copy(a, key, self->max_key_len);
    if(keycpy == NULL) {
        return 1;
    }

    const unsigned long hash = CONCAT(HASH_NAME, _hash)(self, key);
    IndexRecord * node = CONCAT(Indexes, _get)(&self->indexes, hash);
    long values_array_index = -1;

    for(;node->next != NULL; node = node->next) {
        assert(node.value < self->keys.len);
        const char * possible_matching_key = self->keys.items[node.value];
        if(strncmp(key, possible_matching_key, self->max_key_len) == 0) {
            values_array_index = node.value;
            break;
        }
    }

    //create new values array entry if none is found
    if(values_array_index == -1) {
        values_array_index = self->values.len - 1;
        CONCAT(Values, _append)(a, &self->values, value);
        CONCAT(Keys, _append)(a, &self->values, value);
    }
     


    const IndexRecord * new_node = CONCAT(IndexRecord, _cons(a, 

}
#else
;
#endif


#undef HASH_TYPE
#undef HASH_NAME
