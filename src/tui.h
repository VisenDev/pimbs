#ifndef TUI_H
#define TUI_H

#ifdef TUI_IMPLEMENTATION
    #define TUI_IMPLEMENTED
#endif

#include "attributes.h"
#include "format.h"

#define TUI_RED     "\x1b[31m"
#define TUI_GREEN   "\x1b[32m"
#define TUI_YELLOW  "\x1b[33m"
#define TUI_BLUE    "\x1b[34m"
#define TUI_MAGENTA "\x1b[35m"
#define TUI_CYAN    "\x1b[36m"
#define TUI_RESET   "\x1b[0m"

/*Note, a putc function must be defined in order to use the tui module*/
#ifndef PUTCHAR_FUNCTION
#    error "PUTCHAR_FUNCTION must be defined"
#endif /*PUTC_FUNCTION*/

static int tui_put_char(char ch) {
    PUTCHAR_FUNCTION(ch);
    return 1;
}

static int tui_put_str(const char * const str) {
    int i = 0;
    while(str[i] != 0) {
        tui_put_char(str[i]);
        ++i;
    }
    return i;
}

int tui_put_long(long num)
#ifdef TUI_IMPLEMENTATION
{
    char buf[128] = {0};
    format_long(buf, sizeof(buf), num, 10);
    return tui_put_str(buf);
}
#else
;
#endif

int tui_put_double(double num)
#ifdef TUI_IMPLEMENTATION
{
    char buf[128] = {0};
    format_double(buf, sizeof(buf), num, 10);
    return tui_put_str(buf);
}
#else
;
#endif

int tui_put_str_reset(const char * const str) 
#ifdef TUI_IMPLEMENTATION
{
    const int chars = tui_put_str(str); 
    tui_put_str(TUI_RESET);
    return chars;
}
#else
;
#endif

#endif /*TUI_H*/
