#include "concat.h"
#include "attributes.h"
#include "allocator.h"
#include "debug.h"
#include "strutils.h"
#include "errors.h"

/* Ensure include parameters are defined */
#ifndef HASH_TYPE 
#error "HASH_TYPE must be defined before including \"hash.h\""
#endif /*LIST TYPE*/
       
#ifndef HASH_NAME
#error "HASH_NAME must be defined before including \"hash.h\""
#endif /*LIST_NAME*/

/*Bucket definition*/
#define Bucket CONCAT(HASH_NAME, EntryBucket)
typedef struct {
    HASH_TYPE value;
    FixedString key;
} Bucket;

/*Bucket Vec*/
#define BucketVec CONCAT(Bucket, Vec)
#define VEC_NAME BucketVec
#define VEC_TYPE Bucket
#ifdef HASH_IMPLEMENTATION
    #define VEC_IMPLEMENTATION
#endif /*HASH_IMPLEMENTATION*/
#include "vec.h"

/*Sparse set of BucketVec definition*/
#define BucketVecSet CONCAT(BucketVec, Set)
#define SSET_NAME BucketVecSet
#define SSET_TYPE BucketVec
#ifdef HASH_IMPLEMENTATION
    #define SSET_IMPLEMENTATION
#endif /*HASH_IMPLEMENTATION*/
#include "sset.h"

/* Hash table struct definition */
typedef struct HASH_NAME {
    unsigned long modulus;
    BucketVecSet buckets;
} HASH_NAME;

NODISCARD 
HASH_NAME CONCAT(HASH_NAME, _init)(void)
#ifdef HASH_IMPLEMENTATION
{
    HASH_NAME result = {0}; 
    result.modulus = 256;
    result.buckets = CONCAT(BucketVecSet, _init)();
    return result;
}
#else
;
#endif

void CONCAT(HASH_NAME, _free)(Allocator a, HASH_NAME * self)
#ifdef HASH_IMPLEMENTATION
{
    unsigned long i = 0;
    for(i = 0; i < self->buckets.dense.len; ++i) {
        CONCAT(BucketVec, _free)(a, &self->buckets.dense.items[i]);
    }
    CONCAT(BucketVecSet, _free)(a, &self->buckets);
}
#else
;
#endif

void CONCAT(HASH_NAME, _foreach)(HASH_NAME * self, void * ctx, void (*fn)(void * ctx, HASH_TYPE * value))
#ifdef HASH_IMPLEMENTATION
{
    unsigned long i = 0;
    unsigned long j = 0;
    for(i = 0; i < self->buckets.dense.len; ++i) {
        for(j = 0; j < self->buckets.dense.items[i].len; ++j) {
            fn(ctx, &self->buckets.dense.items[i].items[j].value);
        }
    }
}
#else
;
#endif


NODISCARD 
unsigned long CONCAT(HASH_NAME, _hash)(const HASH_NAME * const self, const char * const key, const unsigned long keylen) 
#ifdef HASH_IMPLEMENTATION
{
    /* Inspired by djbt2 by Dan Bernstein - http://www.cse.yorku.ca/~oz/hash.html */
    unsigned long hash = 5381;
    unsigned long i = 0;

    for(i = 0; i < keylen; ++i) {
        const unsigned char c = (unsigned char)key[i];
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return (hash % self->modulus);
}
#else
;
#endif


NODISCARD
HASH_TYPE * CONCAT(HASH_NAME, _get)(HASH_NAME * self, const char * const key)
#ifdef HASH_IMPLEMENTATION
{
    const unsigned long keylen = string_length_fixed(key);
    const unsigned long index = CONCAT(HASH_NAME, _hash)(self, key, keylen);
    const BucketVec * bucketvec = CONCAT(BucketVecSet, _get)(&self->buckets, index);

    /*ensure index exists*/
    if(bucketvec == NULL) {
        return NULL;
    } 

    /*search vec for key*/
    {
        unsigned long i = 0;
        for(i = 0; i < bucketvec->len; ++i) {
            if(string_equal_fixed(bucketvec->items[i].key, key)) {
                return &bucketvec->items[i].value;
            }
        }
    }
    return NULL;
    
}
#else
;
#endif

NODISCARD
int CONCAT(HASH_NAME, _put)(Allocator a, HASH_NAME * self, const char * const key, HASH_TYPE value)
#ifdef HASH_IMPLEMENTATION
{
    /*check if key has an associated value*/
    HASH_TYPE * existing_entry = CONCAT(HASH_NAME, _get)(self, key); 
    if(existing_entry != NULL) {
        *existing_entry = value;
        return ERR_NONE;
    } else {
        /*insert new value*/
        const unsigned long keylen = string_length_fixed(key);
        const unsigned long index = CONCAT(HASH_NAME, _hash)(self, key, keylen);
        BucketVec * bucketvec = CONCAT(BucketVecSet, _get_or_put)(a, &self->buckets, index, CONCAT(BucketVec, _init)());
        if(bucketvec == NULL) {
            tui_printf1("BucketVec for key %s failed to allocate", key);
            return ERR_ALLOCATION_FAILURE;
        } else {
            /*append new key_value pair into vec*/
            int err = ERR_NONE;
            Bucket bucket = {0};
            bucket.key = fixed_string_init(key);
            bucket.value = value;

            err = CONCAT(BucketVec, _append)(a, bucketvec, bucket);
            if(err != ERR_NONE) {
                return err;
            }
            return ERR_NONE;
        }
    }
}
#else
;
#endif

NODISCARD 
int CONCAT(HASH_NAME, _delete)(HASH_NAME * self, const char * const key)
#ifdef HASH_IMPLEMENTATION
{
    const unsigned long keylen = string_length_fixed(key);
    const unsigned long index = CONCAT(HASH_NAME, _hash)(self, key, keylen);
    const int err = CONCAT(BucketVecSet, _delete)(&self->buckets, index);
    return err;
}
#else
;
#endif


NODISCARD 
HASH_TYPE * CONCAT(HASH_NAME, _get_or_put)(Allocator a, HASH_NAME * self, const char * const key, HASH_TYPE fallback_value)
#ifdef HASH_IMPLEMENTATION
{
    HASH_TYPE * found = CONCAT(HASH_NAME, _get)(self, key);
    if(found == NULL) {
        /*const unsigned long keylen = string_length_fixed(key);
        const unsigned long index = CONCAT(HASH_NAME, _hash)(self, key, keylen);*/
        int err = CONCAT(HASH_NAME, _put)(a, self, key, fallback_value);
        if(err != ERR_NONE) {
            log_location();
            /*tui_printf2("get or set sparse set index %lu failed because %s\n", index, error_name(err));*/
            return NULL;
        } 
        found = CONCAT(HASH_NAME, _get)(self, key);
        simple_assert(found != NULL, "found should not be null once it has been set");
        return found;
    } else {
        return found;
    }
}
#else
;
#endif

#undef HASH_NAME
#undef HASH_TYPE
#undef HASH_IMPLEMENTATION
