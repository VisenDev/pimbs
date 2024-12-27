#include "concat.h"
#include "attributes.h"

#ifdef HASH_IMPLEMENTATION
    #include "allocator.h"
    #include "debug.h"
    #include "strutils.h"
#endif /*HASH_IMPLEMENTATION*/

#ifndef HASH_TYPE 
#error "HASH_TYPE must be defined before including \"hash.h\""
#endif /*LIST TYPE*/
       
#ifndef HASH_NAME
#error "HASH_NAME must be defined before including \"hash.h\""
#endif /*LIST_NAME*/

/* Subtypes*/
#ifdef HASH_IMPLEMENTATION
    #define VEC_IMPLEMENTATION
#endif /*HASH_IMPLEMENTATION*/
#define Values CONCAT(HASH_NAME, _values)
#define VEC_NAME Values
#define VEC_TYPE HASH_TYPE
#include "vec.h"

#ifdef HASH_IMPLEMENTATION
    #define VEC_IMPLEMENTATION
#endif /*HASH_IMPLEMENTATION*/
#define Keys CONCAT(HASH_NAME, _keys)
#define VEC_NAME Keys
#define VEC_TYPE char *
#include "vec.h"

#ifdef HASH_IMPLEMENTATION
    #define LIST_IMPLEMENTATION
#endif /*HASH_IMPLEMENTATION*/
#define IndexRecord CONCAT(HASH_NAME, _index_records)
#define LIST_NAME IndexRecord
#define LIST_TYPE unsigned long
#include "list.h"

#ifdef HASH_IMPLEMENTATION
    #define SSET_IMPLEMENTATION
#endif /*HASH_IMPLEMENTATION*/
#define Indexes CONCAT(HASH_NAME, _indexes)
#define SSET_NAME Indexes
#define SSET_TYPE IndexRecord*
#include "sset.h"



NODISCARD MAY_ALLOCATE
static char * string_copy(Allocator a, const char * const str, unsigned long maxlen)
{
    const unsigned long len = string_length(str, maxlen);
    char * buf = a.alloc(a, sizeof(char) * (len + 1));
    unsigned long i = 0;
    if(buf == NULL) {
        return NULL;
    }

    for(i = 0; i < len + 1; ++i) {
        buf[i] = str[i];
    }

    debug_assert(char, buf[len], ==, 0);
    return buf;
}

typedef struct {
    unsigned long max_key_len;
    unsigned long modulus;
    Values values;
    Keys keys;
    Indexes indexes;
} HASH_NAME;

NODISCARD PURE_FUNCTION
static HASH_NAME CONCAT(HASH_NAME, _init)(void) 
#ifdef HASH_IMPLEMENTATION
{
    HASH_NAME result = {0};
    result.max_key_len = 64;
    result.modulus = 1024;
    result.values = CONCAT(Values, _init)();
    result.keys = CONCAT(Keys, _init)();
    result.indexes = CONCAT(Indexes, _init)();
    return result;
}
#else
;
#endif


static void CONCAT(HASH_NAME, _free)(Allocator a, HASH_NAME * self) 
#ifdef HASH_IMPLEMENTATION
{
    unsigned long i = 0;
    for(i = 0; i < self->keys.len; ++i) {
        a.free(a, self->keys.items[i]);
    }

    for(i = 0; i < self->indexes.dense.len; ++i) {
        CONCAT(IndexRecord, _free)(a, self->indexes.dense.items[i]);
    }

    CONCAT(Values, _free)(a, &self->values);
    CONCAT(Keys, _free)(a, &self->keys);
    CONCAT(Indexes, _free)(a, &self->indexes);
}
#else
;
#endif

NODISCARD PURE_FUNCTION
static unsigned long CONCAT(HASH_NAME, _hash)(HASH_NAME * const self, const char * key, unsigned long keylen) 
#ifdef HASH_IMPLEMENTATION
{
    /* Inspired by djbt2 by Dan Bernstein - http://www.cse.yorku.ca/~oz/hash.html */
    unsigned long hash = 5381;
    unsigned long i = 0;

    for(i = 0; i < keylen; ++i) {
        const unsigned char c = (unsigned char)key[i];
        simple_assert(c != 0, "provided keylen is incorrect");
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash % self->modulus;
}
#else
;
#endif


NODISCARD PURE_FUNCTION
static HASH_TYPE * CONCAT(HASH_NAME, _get)(HASH_NAME * self, const char * key, unsigned long keylen) 
#ifdef HASH_IMPLEMENTATION
{
    const unsigned long hash = CONCAT(HASH_NAME, _hash)(self, key, keylen);
    IndexRecord ** maybe_starting_node = CONCAT(Indexes, _get)(&self->indexes, hash);
    IndexRecord * starting_node = maybe_starting_node == NULL ? NULL : *maybe_starting_node; 
    IndexRecord * node = starting_node;
    char * possible_matching_key = NULL;
    long values_array_index = -1;

    for(node = starting_node; node != NULL; node = node->next) {
        debug_assert(unsigned_long, node->value, <, self->keys.len);

        /*extract the key associated with the node*/
        possible_matching_key = self->keys.items[node->value];
        if(string_equal(key, possible_matching_key, self->max_key_len)) {
            values_array_index = (long)node->value;
            break;
        }
    }

    if(values_array_index == -1) {
        return NULL;
    } else {
        debug_assert(long, values_array_index, >=, (long)0);
        debug_assert(unsigned_long, (unsigned long)values_array_index, <, self->values.len);
        return &self->values.items[values_array_index];
    }
} 
#else
;
#endif

NODISCARD
static int CONCAT(HASH_NAME, _put)(Allocator a, HASH_NAME * self, const char * key, unsigned long keylen, const HASH_TYPE value)
#ifdef HASH_IMPLEMENTATION
{
    HASH_TYPE * found = CONCAT(HASH_NAME, _get)(self, key, keylen);

    /*create new values array entry if none is found*/
    if(found == NULL) {
        int err = 0;
        char * keycpy = NULL;

        /*append new entry*/
        err = CONCAT(Values, _append)(a, &self->values, value);
        if(err != ERR_NONE) {
            return ERR_ALLOCATION_FAILURE;
        }

        keycpy = string_copy(a, key, keylen);
        if(keycpy == NULL) {
            return ERR_ALLOCATION_FAILURE;
        }
        err = CONCAT(Keys, _append)(a, &self->keys, keycpy);
        if(err != ERR_NONE) {
            return ERR_ALLOCATION_FAILURE;
        }

        /*create new node*/
        {
            const unsigned long hash = CONCAT(HASH_NAME, _hash)(self, keycpy, keylen);
            IndexRecord ** old_node_location = CONCAT(Indexes, _get)(&self->indexes, hash);
            IndexRecord * old_node = old_node_location == NULL ? NULL : *old_node_location;
            IndexRecord* new_node = CONCAT(IndexRecord, _cons)(a, self->values.len - 1, old_node);
            if(new_node == NULL) {
                return ERR_ALLOCATION_FAILURE;
            }
            err = CONCAT(Indexes, _put)(a, &self->indexes, hash, new_node);
            if(err != ERR_NONE) {
                return ERR_ALLOCATION_FAILURE;
            }
        }

    /*a corresponding key has been found, so update its associated value*/
    } else {
        *found = value;
    }
     
    return ERR_NONE;
}
#else
;
#endif


#undef HASH_TYPE
#undef HASH_NAME
#undef HASH_IMPLEMENTATION
