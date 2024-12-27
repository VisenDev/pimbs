#ifndef STRUTILS_H
#define STRUTILS_H

#include "attributes.h"

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


#endif /*STRUTILS.H*/
