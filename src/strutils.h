#ifndef STRUTILS_H
#define STRUTILS_H

#include "attributes.h"
#include "allocator.h"

typedef struct {
    char * str;
    unsigned long len;
} String;

NODISCARD PURE_FUNCTION
static int string_equal( const char * lhs, const char * rhs, unsigned long maxlen )
{
    unsigned long i = 0;
    for(i = 0; i < maxlen; ++i) {
        const unsigned char lch = (unsigned char)lhs[i];
        const unsigned char rch = (unsigned char)rhs[i];

        if(lch == 0 || rch == 0) {
            return 1;
        }

        if(lch != rch) {
            return 0;
        }
    }

    return 1;
}

NODISCARD PURE_FUNCTION
static unsigned long string_length(const char * const str, const unsigned long maxlen)
{
    unsigned long i = 0;
    for(i = 0; str[i] != 0 && i < maxlen; ++i);
    return i;
}


NODISCARD MAY_ALLOCATE
static char * string_copy(Allocator a, const char * const str, unsigned long maxlen)
{
    const unsigned long len = string_length(str, maxlen);
    char * buf = a.alloc(a, sizeof(char) * (len + 1));
    unsigned long i = 0;
    if(buf == NULL) {
        return NULL;
    }

    for(i = 0; i < len + 1; ++i) {
        buf[i] = str[i];
    }

    debug_assert(char, buf[len], ==, 0);
    return buf;
}

#endif /*STRUTILS.H*/
