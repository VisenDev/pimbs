#include "debug.h"

#define StaticVec(T, cap) struct {T items[cap]; long len; T _tmp; }

#define svec_cap(self) (long)(sizeof(self.items) / sizeof(self.items[0]))

#define svec_append(self, value) do { self.len += 1; svec_set(self, self.len - 1, value); } while (0) 

#define svec_get(self, index) ( \
    inline_assert(index >= 0), \
    inline_assert((long)index < svec_cap(self)), \
    self.items[index])

#define svec_set(self, index, value) \
    do { \
        debug_assert((long)index, >=, 0); \
        debug_assert((long)index, <, (long)svec_cap(self)); \
        self.items[index] = value; \
    } while (0) 

#define svec_swap(self, index_a, index_b) \
    do { \
        self._tmp = svec_get(self, index_a); \
        svec_set(self, index_a, svec_get(self, index_b)); \
        svec_set(self, index_b, self._tmp); \
    } while (0)

#define svec_top(self) (self.items[self.len - 1])

#define svec_pop(self) (\
    inline_assert(self.len >= 1),  \
    self._tmp = svec_top(self), \
    self.len -= 1, \
    self._tmp)

#define svec_remove(self, index) \
    do { \
        long i = 0; \
        debug_assert((long)index, >=, 0); \
        debug_assert((long)index, <, (long)svec_cap(self)); \
        for(i = index + 1; i < self.len; ++i) {\
            svec_swap(self, i, i - 1);\
        } \
        svec_pop(self); \
    } while (0)

#define svec_foreach(self, function, ctx) \
    do { \
        long svec_i = 0; \
        for(svec_i = 0; svec_i < self.len; ++svec_i) { \
            function(ctx, svec_get(self, svec_i)); \
        } \
    } while (0)

