#ifndef HASHMAP_H
#define HASHMAP_H

#include "sparse_set.h"
#include "vector.h"
#include "linked_list.h"
#include "allocator.h"

#ifndef MAXKEYLEN
#define MAXKEYLEN 64 
#endif //MAXKEYLEN

#define IMPLEMENT_HASHMAP(Name, Type) \
\
IMPLEMENT_VECTOR(Name##Type##Vec, Type) \
IMPLEMENT_VECTOR(Name##Keys, char *) \
IMPLEMENT_LINKED_LIST(Name##Index, unsigned long) \
IMPLEMENT_SPARSE_SET(Name##Indexes, Name##Index) \
\
typedef struct { \
    Name##Type##Vec items; \
    Name##Keys keys; \
    Name##Indexes indexes; \
    unsigned long modulus; \
} Name; \
\
static Name Name##_init(Allocator a) { \
    return (Name) { \
        .items = Name##Type##Vec_init(a), \
        .keys = Name##Keys_init(a), \
        .indexes = Name##Indexes_init(a), \
        .modulus = 256, \
    }; \
} \
\
static void Name##_free(Allocator a, Name * self) { \
    Name##Type##Vec_free(a, &self->items); \
    Name##Keys_free(a, &self->keys); \
    Name##Indexes_free(a, &self->indexes); \
    self->items = NULL; \
    self->keys = NULL; \
    self->indexes = NULL; \
} \
static unsigned long Name##_hash(const char * const str, const size_t maxlen) { \
    /*Inspired by djbt2 by Dan Bernstein - http://www.cse.yorku.ca/~oz/hash.html */ \
    unsigned long hash = 5381; \
    unsigned long c = 0; \
\
    for(size_t i = 0; (i < maxlen) && (str[i] != 0); ++i) { \
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */ \
    } \
\
    return hash; \
} \
\
static unsigned long Name##_calculate_index(const Name * const self, const char * const key) { \
    unsigned long hash = Name##_hash(key, MAXKEYLEN); \
    return hash % self->modulus; \
} \
\
void Name##_put(const Allocator a, Name * self, const char * const key, Type value) { \
    unsigned long sparse_index = Name##_calculate_index(self, key); \
    unsigned long item






    

#endif //HASHMAP_H
