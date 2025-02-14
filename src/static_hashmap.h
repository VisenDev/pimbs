
/*
#define StaticString(maxlen) struct {char buf[maxlen]; long len;}



#define sstr_init(string)
*/


#include "strutils.h"

typedef struct {
    long key_index;
    long value_index;
} BucketValue;

#define StaticHashmap(T, max_entries) \
    struct { \
        StaticVec(FixedString, max_entries) keys; \
        StaticVec(T, max_entries) values; \
        StaticVec(BucketValue, max_entries) lookup_table; \
    }

void shash_put_internal(

#define shash_put(self, keystr, value) \

