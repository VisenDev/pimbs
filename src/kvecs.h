/*
typedef struct {
    unsigned int signed : 1;
    unsigned long long integer;
    unsigned long long decimal;
} kv_Number;

typedef struct {
    char * ptr;
    unsigned long len;
} kv_String;

typedef struct {
    unsigned int value : 1;
} kv_Boolean;

 Main types
typedef union {
    kv_Number number;
    kv_String string;
    kv_Boolean boolean;
} kv_Value;

typedef enum {
    kv_Tag_NUMBER, 
    kv_Tag_STRING, 
    kv_Tag_BOOLEAN, 
} kv_Tag;

typedef struct {
    kv_Tag tag;
    kv_Value value;
} kv_TaggedValue;
*/


#ifndef KVECS_TYPE 
    #error "KVECS_TYPE must be defined before including \"kvecs.h\""
#endif 
       
#ifndef KVECS_NAME
    #error "KVECS_NAME must be defined before including \"kvecs.h\""
#endif

#include "attributes.h"
#include "allocator.h"
#include "debug.h"
#include "concat.h"


#define Id CONCAT(KVECS_NAME, Id)
typedef struct {
    long index;
} Id;
#define NULL_ID {-1}

#define ValueUnion CONCAT(KVECS_NAME, ValueUnion)
#define Value CONCAT(KVECS_NAME, Value)

typedef union {
    Id child;    
    KVECS_TYPE value;
} ValueUnion

typedef struct {
    int is_child;
    ValueUnion data;
} Value;


#define ValueSet CONCAT(KVECS_NAME, Set)
#define SSET_NAME ValueSet
#define SSET_TYPE Value
#ifdef KVECS_IMPLEMENTATION
    #define SSET_IMPLEMENTATION
#endif
#include "sset.h"

#define IdVec CONCAT(KVECS_NAME, IdVec)
#define VEC_NAME IdVec
#define VEC_TYPE Id
#ifdef KVECS_IMPLEMENTATION
    #define VEC_IMPLEMENTATION
#endif
#include "vec.h"

#define Components CONCAT(KVECS_NAME, Components)
#define HASH_NAME Components
#define HASH_TYPE ValueSet
#ifdef KVECS_IMPLEMENTATION
    #define HASH_IMPLEMENTATION
#endif
#include "../3rdparty/pimbs/src/hash.h"


/* Ecs */
typedef struct {
    Id max_id;
    IdVec free_ids; 
    Components components;
} KVECS_NAME;


NODISCARD
KVECS_NAME CONCAT(KVECS_NAME, _init)(void)
#ifdef KVECS_IMPLEMENTATION
{
    KVECS_NAME result = {0};
    result.free_ids = CONCAT(IdVec, _init)();
    result.max_id = {0};
    result.components = CONCAT(Components, _init)();
    return result;
}
#else
;
#endif

NODISCARD 
int CONCAT(KVECS_NAME, _allocate_more_ids)(Allocator a, Ecs * self)
#ifdef KVECS_IMPLEMENTATION
{

    const old_max = self->max_id;
    unsigned long i = 0;
    self->max_id.index = (self->max_id.index + 1) * 2;
    for(i = old_max.index; i < self->max_id.index; ++i) {
        Id new_id = {0};
        new_id.index = (long)i;
        const int err = CONCAT(IdVec, _append)(a, &self->free_ids, new_id);
        if(err != ERR_NONE) {
            return err;
        }
    }
    return ERR_NONE;
}
#else
;
#endif

NODISCARD
Id CONCAT(KVECS_NAME, _new_entity)(KVECS_NAME * self, Allocator a) 
#ifdef KVECS_IMPLEMENTATION
{
    Id * top = CONCAT(IdVec, _top)(&self->free_ids);
    if(top == NULL) {
        const int err = CONCAT(a, &self->free_ids, 32, 
        if(err != ERR_NONE) {
            return NULL_ID;
        }
        simple_assert(top != NULL, "getting new free id failed");
    }
    return SAFE_DEREF(CONCAT(VEC_NAME, _swap_pop_top)(self));

}
#else
;
#endif

NODISCARD 
int CONCAT(KVECS_NAME, _set)(Allocator a, KVECS_NAME * self, Id id, char * key, KVECS_TYPE value)
#ifdef KVECS_IMPLEMENTATION
{

}
#else
;
#endif
