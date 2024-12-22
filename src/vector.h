#ifndef VECTOR_H
#define VECTOR_H

#define IMPLEMENT_VECTOR(Name, Type) \
    typedef struct { \
        Type * items; \
        size_t len; \
        size_t cap; \
    } Name; \
    \
    Name Name##_init(Allocator a) { \
        return (Name){ \
            .items = a.alloc(a.ctx, 4 * sizeof(Type)), \
            .len = 0, \
            .cap = 4, \
        }; \
    } \
    \
    void Name##_deinit(Allocator a, Name * vec) { \
        a.free(a.ctx, vec->items); \
        vec->len = 0; \
        vec->cap = 0; \
        vec->items = NULL; \
    } \
    \
    void Name##_append(Allocator a, Name * vec, Type value) { \
        if(vec->len < vec->cap) { \
            vec->items[vec->len] = value; \
            vec->len += 1; \
        } else { \
            const size_t new_cap = (vec->cap + 1) * 2; \
            a.realloc(a.ctx, vec->items, new_cap); \
            vec->cap = new_cap; \
            Name##_append(a, vec, value); \
        } \
    }

#endif //VECTOR_H
