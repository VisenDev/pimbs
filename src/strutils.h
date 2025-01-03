#ifndef STRUTILS_H
#define STRUTILS_H

#include "attributes.h"

typedef struct {
    char * str;
    unsigned long len;
} String;

NODISCARD 
static int string_equal(const char * const lhs, const char * const rhs, unsigned long maxlen )
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

NODISCARD 
static unsigned long string_length(const char * const str, const unsigned long maxlen)
{
    unsigned long i = 0;
    for(i = 0; str[i] != 0 && i < maxlen; ++i);
    return i;
}


/* memcpy */
static void memory_copy(void * dest, const void * const src, const unsigned long byte_count)
{
    char * destbuf = dest;
    const char * const srcbuf = src;
    unsigned long i = 0;
    for(i = 0; i < byte_count; ++i) {
        destbuf[i] = srcbuf[i];
    }
}


/* Fixed length string */
#ifndef FIXED_STRING_LEN
    #define FIXED_STRING_LEN 64
#endif

typedef struct {
    char str[FIXED_STRING_LEN];
    unsigned long len;
} FixedString;

NODISCARD 
static unsigned long string_length_fixed(const char * const str) {
    return string_length(str, FIXED_STRING_LEN - 1);
}

NODISCARD 
static int string_equal_fixed(const FixedString lhs, const char * const rhs) {
    return string_equal(lhs.str, rhs, lhs.len);
}

NODISCARD 
static FixedString fixed_string_init(const char * const str) {
    const unsigned long len = string_length_fixed(str);
    FixedString result = {0};
    result.len = len;
    memory_copy(&result.str, str, len);
    return result;
}

#endif /*STRUTILS.H*/
