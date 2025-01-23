#include "debug.h"

#define StaticVec(T, cap) struct {T items[cap]; unsigned long len; T _tmp; }

#define svec_cap(self) (unsigned long)(sizeof(self.items) / sizeof(self.items[0]))
#define svec_append(self, value) (self.len += 1, svec_set(self, self.len - 1, value))
#define svec_get(self, index) ( \
    inline_assert(index >= 0), \
    inline_assert((unsigned long)index < svec_cap(self)), \
    self.items[index])
#define svec_set(self, index, value) (\
    inline_assert(index >= 0), \
    inline_assert(index < svec_cap(self)), \
    self.items[index] = value)
#define svec_swap(self, index_a, index_b) (\
    self._tmp = svec_get(self, index_a), \
    svec_set(self, index_a, svec_get(self, index_b)) \
    svec_set(self, index_b, self._tmp))
#define svec_top(self) (self.items[self.len - 1])
#define svec_pop(self) (\
    inline_assert(self.len >= 1),  \
    self._tmp = svec_top(self), \
    self.len -= 1, \
    self._tmp)
#define svec_remove(self, index) ( \
    do { \
        unsigned long svec_i = 0; \
        inline_assert(index >= 0); \
        inline_assert(index < svec_cap(self)); \
        for(svec_i = index + 1; svec_i < self.len; ++svec_i) {\
            svec_swap(self, svec_i, svec_i - 1)\
        } \
        svec_pop(self); \
    } while (0)
#define svec_foreach(self, function, ctx) \
    do { \
        unsigned long svec_i = 0; \
        for(svec_i = 0; svec_i < self.len; ++svec_i) { \
            function(ctx, svec_get(self, svec_i)); \
        } \
    } while (0)

