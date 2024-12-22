#include "allocator.h"

#ifndef LIST_TYPE 
#error "LIST_TYPE must be defined before including \"list.h\""
#endif //LIST TYPE
       
#ifndef LIST_NAME
#error "LIST_NAME must be defined before including \"vector.h\""
#endif //LIST_NAME
       
 // Indirection to force expansion
#define CONCATENATE_DETAIL(x, y) x##y
#define CONCAT(x, y) CONCATENATE_DETAIL(x, y)

typedef struct LIST_NAME {
    LIST_TYPE value;
    struct LIST_NAME * next;
} LIST_NAME;

LIST_NAME * CONCAT(LIST_NAME, _cons)(Allocator a, LIST_TYPE value, LIST_NAME * next)
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

void CONCAT(LIST_NAME, _free)(Allocator a, LIST_NAME * start)
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