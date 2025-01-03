#ifndef TUI_H
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
    #if defined(USE_STDLIB) && USE_STDLIB == 1
        #include <stdio.h>
        #define LOG_FUNCTION printf
    #else
        static int dummy_log_function(const char * format, ...) {
            (void)format;
            return 0;
        }
        #define LOG_FUNCTION dummy_log_function
    #endif /*USE_STDLIB*/
#endif /*LOG_FUNCTION*/


/* C89 doesn't have variadic macros :( */
#define tui_printf(fmt)                 do {LOG_FUNCTION(fmt);                LOG_FUNCTION(TUI_RESET);} while(0)
#define tui_printf1(fmt, a)             do {LOG_FUNCTION(fmt, a);             LOG_FUNCTION(TUI_RESET);} while(0)
#define tui_printf2(fmt, a, b)          do {LOG_FUNCTION(fmt, a, b);          LOG_FUNCTION(TUI_RESET);} while(0)
#define tui_printf3(fmt, a, b, c)       do {LOG_FUNCTION(fmt, a, b, c);       LOG_FUNCTION(TUI_RESET);} while(0)
#define tui_printf4(fmt, a, b, c, d)    do {LOG_FUNCTION(fmt, a, b, c, d);    LOG_FUNCTION(TUI_RESET);} while(0)
#define tui_printf5(fmt, a, b, c, d, e) do {LOG_FUNCTION(fmt, a, b, c, d, e); LOG_FUNCTION(TUI_RESET);} while(0)


/*TODO:
 * define all ansi escape codes
 * implement print function
 */


#endif /*TUI_H*/
