#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

/*NODISCARD*/
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311/*L*/) /*check for C23*/
    #define NODISCARD [[nodiscard]]
#elif defined(__gcc__) || defined(__clang__)
    #define NODISCARD __attribute__((warn_unused_result))
#else
    #define NODISCARD
#endif /*NODISCARD*/

#if defined(__gcc__) || defined(__clang__)
    #define LIKELY_TRUE(expr)  __builtin_expect(expr, 1)
    #define LIKELY_FALSE(expr) __builtin_expect(expr, 0)
    #define ABORT() __builtin_trap()
    #define PURE_FUNCTION __attribute__((const))
#else
    #define LIKELY_TRUE(expr) expr
    #define LIKELY_FALSE(expr) expr
    #define PURE_FUNCTION
    #if USE_STDLIB == 1
        #include <stdlib.h>
        #define ABORT() abort()
    #else 
        #define ABORT() (*(volatile int *)0) = 0
    #endif /*USE_STDLIB*/
#endif /*GCC or Clang*/


/*ALIGNOF*/
#if defined(__GNUC__) || defined(__clang__)
    #define ALIGNOF(type) __alignof__(type)
#elif defined(_MSC_VER)
    #define ALIGNOF(type) __alignof(type)
#else
    #define ALIGNOF(type) ((size_t)&((struct { char c; type member; } *)0)->member)
#endif /*ALIGNOF*/

/*NULL*/
#ifndef NULL
    #define NULL ((void *)0)
#endif /*NULL*/


#endif /*ATTRIBUTES_H*/
