#ifndef KVECS_TYPE 
    #error "KVECS_TYPE must be defined before including \"kvecs.h\""
#endif 
       
#ifndef KVECS_NAME
    #error "KVECS_NAME must be defined before including \"kvecs.h\""
#endif

#ifndef KVECS_H
#define KVECS_H

    #include "attributes.h"
    #include "allocator.h"
    #include "debug.h"
    #include "concat.h"

    typedef struct {
        long index;
    } Id;
    #define NULL_ID -1
    #define UNPACK_ID(id) (inline_assert(id.index != NULL_ID), id.index)

#endif /*KVECS_H*/

#define ValueSet CONCAT(KVECS_NAME, ValueSparseSet)
#define SSET_NAME ValueSet
#define SSET_TYPE KVECS_TYPE 
#ifdef KVECS_IMPLEMENTATION
    #define SSET_IMPLEMENTATION
#endif
#include "sset.h"

#define ChildSet CONCAT(KVECS_NAME, ChildSparseSet)
#define SSET_NAME ChildSet
#define SSET_TYPE Id
#ifdef KVECS_IMPLEMENTATION
    #define SSET_IMPLEMENTATION
#endif
#include "sset.h"

#define IdVec CONCAT(KVECS_NAME, IdVector)
#define VEC_NAME IdVec
#define VEC_TYPE Id
#ifdef KVECS_IMPLEMENTATION
    #define VEC_IMPLEMENTATION
#endif
#include "vec.h"

#define Components CONCAT(KVECS_NAME, ComponentHashtable)
#define HASH_NAME Components
#define HASH_TYPE ValueSet
#ifdef KVECS_IMPLEMENTATION
    #define HASH_IMPLEMENTATION
#endif
#include "hash.h"


#define Children CONCAT(KVECS_NAME, ChildrenHashtable)
#define HASH_NAME Children
#define HASH_TYPE ChildSet
#ifdef KVECS_IMPLEMENTATION
    #define HASH_IMPLEMENTATION
#endif
#include "hash.h"



/* Ecs */
typedef struct {
    Id max_id;
    IdVec free_ids; 
    Components components;
    Children children;
} KVECS_NAME;


NODISCARD
KVECS_NAME CONCAT(KVECS_NAME, _init)(void)
#ifdef KVECS_IMPLEMENTATION
{
    KVECS_NAME result = {0};
    result.free_ids = CONCAT(IdVec, _init)();
    /*result.max_id = {0};*/
    result.components = CONCAT(Components, _init)();
    result.children = CONCAT(Children, _init)();
    return result;
}
#else
;
#endif

NODISCARD 
int CONCAT(KVECS_NAME, _allocate_more_ids)(Allocator a, KVECS_NAME * self)
#ifdef KVECS_IMPLEMENTATION
{

    const Id old_max = self->max_id;
    long i = 0;
    self->max_id.index = (self->max_id.index + 1) * 2;
    for(i = old_max.index; i < self->max_id.index; ++i) {
        Id new_id = {0};
        new_id.index = i;
        {
            const int err = CONCAT(IdVec, _append)(a, &self->free_ids, new_id);
            if(err != ERR_NONE) {
                return err;
            }
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
        const int err = CONCAT(KVECS_NAME, _allocate_more_ids)(a, self);
        if(err != ERR_NONE) {
            Id result = {NULL_ID};
            return result;
        }
        simple_assert(top != NULL, "getting new free id failed");
    }
    /*TODO fix this code*/
    return SAFE_DEREF(CONCAT(VEC_NAME, _swap_pop_top)(&self->free_ids));

}
#else
;
#endif

NODISCARD 
int CONCAT(KVECS_NAME, _set)(Allocator a, KVECS_NAME * self, Id id, char * key, KVECS_TYPE value)
#ifdef KVECS_IMPLEMENTATION
{
    ValueSet * set = CONCAT(Components, _get_or_put)(&self->components, key);
    if(set == NULL) {
        return ERR_ALLOCATION_FAILURE;
    } else {
        const int err = CONCAT(ValueSet, _put)(a, set, UNPACK_ID(id), value);
        return err;
    }

}
#else
;
#endif

NODISCARD 
KVECS_TYPE * CONCAT(KVECS_NAME, _get)(KVECS_NAME * self, Id id, char * key)
#ifdef KVECS_IMPLEMENTATION
{
    ValueSet * set = CONCAT(Components, _get)(&self->components, key);
    if(set == NULL) {
        return NULL;
    } else {
        return CONCAT(ValueSet, _get)(set, UNPACK_ID(id));
    }

}
#else
;
#endif

NODISCARD
Id CONCAT(KVECS_NAME, _set_child)(Allocator a, KVECS_NAME * self, Id parent, char * key) 
#ifdef KVECS_IMPLEMENTATION
{
    ChildSet * set = CONCAT(Children, _get_or_put)(&self->children, key);
    if(set == NULL) {
        return {NULL_ID};
    } else {
        const Id child = CONCAT(KVECS_NAME, _new_entity)(a, self);
        if(child.index == NULL_ID) {
            return {NULL_ID};
        } else {
            const int err = CONCAT(ChildSet, _put)(a, set, UNPACK_ID(id), child);
        }
    }
}
#else
;
#endif


NODISCARD 
Id * CONCAT(KVECS_NAME, _get_child)(KVECS_NAME * self, Id parent, char * key)
#ifdef KVECS_IMPLEMENTATION
{
    ChildSet * set = CONCAT(Children, _get)(&self->children, key);
    if(set == NULL) {
        return NULL;
    } else {
        return CONCAT(ChildSet, _get)(set, UNPACK_ID(parent));
    }

}
#else
;
#endif
