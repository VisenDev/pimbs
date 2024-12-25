#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 202311L) //check for C23
    #define NODISCARD [[nodiscard]]
//    #define PURE_FUNCTION [[reproducable]]
#elif defined(__GNUC__) || defined(__clang__)
    #define NODISCARD __attribute__((warn_unused_result))
 //   #define PURE_FUNCTION __attribute__((const))
#else
    #define NODISCARD
  //  #define PURE_FUNCTION
#endif //C23

#if defined(__GNUC__) || defined(__clang__)
    #define LIKELY_TRUE(expr)  __builtin_expect(expr, 1)
    #define LIKELY_FALSE(expr) __builtin_expect(expr, 0)
    #define MAY_ALLOCATE __attribute__((malloc))
    #define ABORT() __builtin_trap()
    #define PURE_FUNCTION __attribute__((const))
#else
    #define LIKELY_TRUE(expr) expr
    #define LIKELY_FALSE(expr) expr
    #define MAY_ALLOCATE
    #define PURE_FUNCTION
    #ifdef NO_CSTDLIB
        #define ABORT() (*(volatile int *)0) = 0
    #else 
        #define ABORT() abort()
    #endif //NO_CSTDLIB
#endif //GCC or Clang

#endif //ATTRIBUTES_H
