/***************CHECK DEPENDENCIES*************/
//#ifndef CONCAT_H
//    #error "\"concat.h\" must be included before \"list.h\""
//#endif //CONCAT_H
//
//
//#ifndef ALLOCATOR_H
//    #error "\"allocator.h\" must be included before \"list.h\""
//#endif //ALLOCATOR_H
#include "concat.h"
#include "attributes.h"

#ifdef LIST_IMPLEMENTATION
    #include "allocator.h"
#endif

/**************NAME AND TYPE*******************/
#ifndef LIST_TYPE 
    #error "LIST_TYPE must be defined before including \"list.h\""
#endif //LIST TYPE
       
#ifndef LIST_NAME
    #error "LIST_NAME must be defined before including \"list.h\""
#endif //LIST_NAME
       

/*****************IMPLEMENTATION**************/
struct LIST_NAME;

#ifdef __clang__
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wpadded"
#endif //__clang__
typedef struct LIST_NAME {
    struct LIST_NAME * next;
    LIST_TYPE value;
} LIST_NAME;
#pragma clang diagnostic pop

NODISCARD 
static LIST_NAME * CONCAT(LIST_NAME, _cons)(Allocator a, LIST_TYPE value, struct LIST_NAME * next)
#ifdef LIST_IMPLEMENTATION
{
    LIST_NAME * node = a.alloc(a, sizeof(LIST_NAME)); 
    if(node == NULL) {
        return NULL;
    }
    node->value = value;
    node->next = next;
    return node;
}
#else
;
#endif

static void CONCAT(LIST_NAME, _free)(Allocator a, LIST_NAME * start)
#ifdef LIST_IMPLEMENTATION
{
    if(start != NULL) {
        LIST_NAME * next = start->next;
        a.free(a, start);
        CONCAT(LIST_NAME, _free) (a, next);
    }
}
#else
;
#endif


#undef LIST_TYPE
#undef LIST_NAME
#undef LIST_IMPLEMENTATION
