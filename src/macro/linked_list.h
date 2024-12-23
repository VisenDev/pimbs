#ifndef LINKED_LIST_H 
#define LINKED_LIST_H

#define IMPLEMENT_LINKED_LIST(Name, Type) \
struct Name##Struct { \
    Type value; \
    struct Name##Struct* next; \
};  \
typedef struct Name##Struct Name; \
\
Name * Name##_cons(Allocator a, Type value, Name* next) { \
    Name * result = a.alloc(a.ctx, sizeof(Name)); \
    result->value = value; \
    result->next = next; \
    return result; \
} \
\
void Name##_free(Allocator a, Name* node) { \
    Name * next = node->next; \
    a.free(a.ctx, node); \
    if(next) { \
       Name##_free(a, next); \
    } \
} \

#endif //LINkED_LIST_H
