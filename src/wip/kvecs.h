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
    #define UNPACK_ID(id) (inline_assert(id.index != NULL_ID), (unsigned long)id.index)

    static const Id null_id = {NULL_ID};

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
    result.max_id = null_id;
    result.components = CONCAT(Components, _init)();
    result.children = CONCAT(Children, _init)();
    return result;
}
#else
;
#endif


void CONCAT(KVECS_NAME, _free)(Allocator a, KVECS_NAME * self)
#ifdef KVECS_IMPLEMENTATION
{
    unsigned long i = 0;
    unsigned long j = 0;
    CONCAT(IdVec, _free)(a, &self->free_ids);
    
    for(i = 0; i < self->components.buckets.dense.len; ++i) {
        for(j = 0; j < self->components.buckets.dense.items[i].len; ++j) {
            CONCAT(ValueSet, _free)(a, &self->components.buckets.dense.items[i].items[j].value);
        }
    }

    for(i = 0; i < self->children.buckets.dense.len; ++i) {
        for(j = 0; j < self->children.buckets.dense.items[i].len; ++j) {
            CONCAT(ChildSet, _free)(a, &self->children.buckets.dense.items[i].items[j].value);
        }
    }

    CONCAT(Components, _free)(a, &self->components);  
    CONCAT(Children, _free)(a, &self->children);
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
    self->max_id.index = (self->max_id.index + 5) * 2;
    for(i = old_max.index; i < self->max_id.index; ++i) {
        Id new_id = {0};
        new_id.index = i;
        {
                tui_printf("\nappending new ids\n");
                {
            const int err = CONCAT(IdVec, _append)(a, &self->free_ids, new_id);
            if(err != ERR_NONE) {
                return err;
            }
                }
        }
    }
    return ERR_NONE;
}
#else
;
#endif

NODISCARD
Id CONCAT(KVECS_NAME, _new_entity)(Allocator a, KVECS_NAME * self)
#ifdef KVECS_IMPLEMENTATION
{
    Id * top = CONCAT(IdVec, _pop)(&self->free_ids);
    if(top == NULL) {
        const int err = CONCAT(KVECS_NAME, _allocate_more_ids)(a, self);
        if(err != ERR_NONE) {
            Id result = {NULL_ID};
            return result;
        }
        top = CONCAT(IdVec, _pop)(&self->free_ids);
        simple_assert(top != NULL, "getting new free id failed");
    }
    return *top;
}
#else
;
#endif

NODISCARD 
int CONCAT(KVECS_NAME, _set)(Allocator a, KVECS_NAME * self, Id id, const char * const key, KVECS_TYPE value)
#ifdef KVECS_IMPLEMENTATION
{
    ValueSet def = CONCAT(ValueSet, _init)();
    ValueSet * set = CONCAT(Components, _get_or_put)(a, &self->components, key, def);
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
KVECS_TYPE * CONCAT(KVECS_NAME, _get)(KVECS_NAME * self, Id id, const char * const key)
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
Id CONCAT(KVECS_NAME, _set_child)(Allocator a, KVECS_NAME * self, Id parent, const char * const key) 
#ifdef KVECS_IMPLEMENTATION
{
    ChildSet dflt = CONCAT(ChildSet, _init)();
    ChildSet * set = CONCAT(Children, _get_or_put)(a, &self->children, key, dflt);
    if(set == NULL) {
        return null_id;
    } else {
        const Id child = CONCAT(KVECS_NAME, _new_entity)(a, self);
        if(child.index == NULL_ID) {
            return null_id;
        } else {
            const int err = CONCAT(ChildSet, _put)(a, set, UNPACK_ID(parent), child);
            if(err != ERR_NONE) {
                return null_id;
            }
            return child;
        }
    }
}
#else
;
#endif


NODISCARD 
Id * CONCAT(KVECS_NAME, _get_child)(KVECS_NAME * self, Id parent, const char * const key)
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
