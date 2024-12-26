#if !defined(TUI_H) || (defined(TUI_IMPLEMENTATION) && !defined(TUI_IMPLEMENTED))
#define TUI_H
#ifdef TUI_IMPLEMENTATION
   #define TUI_IMPLEMENTED
#endif

#include "attributes.h"

#define TUI_RED     "\x1b[31m"
#define TUI_GREEN   "\x1b[32m"
#define TUI_YELLOW  "\x1b[33m"
#define TUI_BLUE    "\x1b[34m"
#define TUI_MAGENTA "\x1b[35m"
#define TUI_CYAN    "\x1b[36m"
#define TUI_RESET   "\x1b[0m"



#ifndef LOG_FUNCTION
    PURE_FUNCTION
    static inline int dummy_log_function(const char * format, ...) {
        (void)format;
        return 0;
    }
    #define LOG_FUNCTION(...) dummy_log_function(__VA_ARGS__)
#endif //LOG_FUNCTION
       
//static inline int tui_printf(

#define tui_printf(format, ...) LOG_FUNCTION(format TUI_RESET, __VA_ARGS__)
#define debug_printf tui_printf

#endif //TUI_H
