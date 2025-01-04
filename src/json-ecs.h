#ifndef KVECS_H
#define KVECS_H

#include "attributes.h"
#include "allocator.h"
#include "debug.h"
#include "concat.h"

typedef struct {
    long index;
} kv_id;
#define KV_NULL_ID -1
#define KV_UNPACK_ID(id) (inline_assert(id.index != KV_NULL_ID), (unsigned long)id.index)

static const kv_id kv_null_id = {KV_NULL_ID};

struct kv_string {
    char * ptr;
    unsigned long len;
};

struct kv_value {
    union kv_union {
        long integer;
        double real;
        int boolean;
        struct kv_string string;
        kv_id object;
        /*todo think thru array*/
    } value;
    enum kv_type {
        KV_INTEGER,
        KV_REAL,
        KV_BOOLEAN,
        KV_STRING,
        KV_OBJECT
    } tag;
};


#define SSET_NAME kv_sset
#define SSET_TYPE struct kv_value 
#ifdef KVECS_IMPLEMENTATION
    #define SSET_IMPLEMENTATION
#endif
#include "sset.h"

#define VEC_NAME kv_id_vec
#define VEC_TYPE kv_id
#ifdef KVECS_IMPLEMENTATION
    #define VEC_IMPLEMENTATION
#endif
#include "vec.h"

#define HASH_NAME kv_components
#define HASH_TYPE kv_sset
#ifdef KVECS_IMPLEMENTATION
    #define HASH_IMPLEMENTATION
#endif
#include "hash.h"



/* Ecs */
struct kv_ecs {
    unsigned long max_id;
    kv_id_vec free_ids; 
    kv_components components;
} ;


#endif /*KVECS_H*/


NODISCARD
struct kv_ecs kv_ecs_init(void)
#ifdef KVECS_IMPLEMENTATION
{
    struct kv_ecs result = {0};
    result.max_id = 0;
    result.free_ids = kv_id_vec_init();
    result.components = kv_components_init();
    return result;
}
#else
;
#endif

#ifdef KVECS_IMPLEMENTATION
/*created so it can be passed as a fn pointer to hash_foreach*/
void kv_sset_free_wrapper(void * ctx, kv_sset * value) {
    kv_sset_free(*(Allocator *)ctx, value);
}
#endif

void kv_ecs_free(Allocator a, struct kv_ecs * self)
#ifdef KVECS_IMPLEMENTATION
{
    kv_id_vec_free(a, &self->free_ids);
    kv_components_foreach(&self->components, &a, kv_sset_free_wrapper);
    kv_components_free(a, &self->components);
}
#else
;
#endif


NODISCARD 
int kv_ecs_allocate_more_ids(Allocator a, struct kv_ecs * self)
#ifdef KVECS_IMPLEMENTATION
{

    const unsigned long old_max = self->max_id;
    unsigned long i = 0;
    int err = ERR_NONE;
    self->max_id = (self->max_id + 8) * 2;

    for(i = old_max; i < self->max_id; ++i) {
        kv_id new_id = {0};
        new_id.index = (long)i;
        err = kv_id_vec_append(a, &self->free_ids, new_id);
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
kv_id kv_ecs_new_entity(Allocator a, struct kv_ecs * self)
#ifdef KVECS_IMPLEMENTATION
{
    kv_id * top = kv_id_vec_pop(&self->free_ids);
    if(top == NULL) {
        const int err = kv_ecs_allocate_more_ids(a, self);
        if(err != ERR_NONE) {
            return kv_null_id;
        }
        top = kv_id_vec_pop(&self->free_ids);
        simple_assert(top != NULL, "getting new free id failed");
    }
    return *top;
}
#else
;
#endif

    NODISCARD 
int kv_ecs_set(Allocator a, struct kv_ecs * self, kv_id id, const char * const key, struct kv_value value)
#ifdef KVECS_IMPLEMENTATION
{
    struct kv_sset default_set = kv_sset_init();
    struct kv_sset * set = kv_components_get_or_put(a, &self->components, key, default_set);
    if(set == NULL) {
        return ERR_ALLOCATION_FAILURE;
    } else {
        const int err = kv_sset_put(a, set, KV_UNPACK_ID(id), value);
        return err;
    }

}
#else
;
#endif

    NODISCARD 
struct kv_value * kv_ecs_get(struct kv_ecs * self, kv_id id, const char * const key)
#ifdef KVECS_IMPLEMENTATION
{
    struct kv_sset * set = kv_components_get(&self->components, key);
    if(set == NULL) {
        return NULL;
    } else {
        return kv_sset_get(set, KV_UNPACK_ID(id));
    }

}
#else
;
#endif

NODISCARD
kv_id * kv_ecs_parse_json(Allocator a, struct kv_ecs * self, const char * const json, const unsigned long len)
#ifdef KVECS_IMPLEMENTATION
{
    kv_id_vec push_down_automata = kv_id_vec_init();
    unsigned long i = 0;

    





}
#else
;
#endif
