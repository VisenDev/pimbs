#ifndef DEBUG_H
#define DEBUG_H

#include "tui.h"
#include "attributes.h"

static int log_location_internal(const char * file, const long line) {
    return
    tui_put_str("   FILE: ") +
    tui_put_str(file) +
    tui_put_str("\n    LINE: ") +
    tui_put_long(line) +
    tui_put_str("\n\n");
}

#define debug_log_location() log_location_internal(__FILE__, __LINE__)

static void debug_assert_internal(
        const int condition,
        const char * lhs_str,
        const char * rhs_str,
        const char * operator_str,
        const long lhs,
        const long rhs,
        const char * file,
        const int line
        ) {
    if(LIKELY_FALSE(!condition)) {
        {
            tui_put_str(TUI_RED"\nASSERTION FAILURE: ");
            tui_put_str(lhs_str);
            tui_put_str(" ");
            tui_put_str(operator_str);
            tui_put_str(" ");
            tui_put_str(rhs_str);
            tui_put_str("\n");
        }

        {
            tui_put_str(TUI_YELLOW"   expression \"");
            tui_put_str(lhs_str);
            tui_put_str("\" expands to ");
            tui_put_long(lhs);
            tui_put_str("\n");
        }

        {
            tui_put_str(TUI_YELLOW"   expression \"");
            tui_put_str(rhs_str);
            tui_put_str("\" expands to ");
            tui_put_long(rhs);
            tui_put_str_reset("\n");
        }

        {
            tui_put_str("   FILE: ");
            tui_put_str(file);
            tui_put_str("\n    LINE: ");
            tui_put_long(line);
            tui_put_str("\n\n");
        }

        tui_put_str_reset(TUI_RED   "   ABORTING...\n\n"); \
        ABORT();

    }
}

#define simple_assert(condition, msg) \
    do { \
        if(LIKELY_FALSE(!(condition))) { \
            tui_put_str_reset(TUI_RED "\nASSERTION FAILED\n"); \
            tui_put_str(msg); \
            tui_put_str("\n"); \
            ABORT(); \
        } \
    } while (0) \

#define debug_assert(lhs, operator, rhs) \
    debug_assert_internal((lhs operator rhs), #lhs, #operator, #rhs, lhs, rhs, __FILE__, __LINE__)

#define inline_assert(expression) \
    LIKELY_TRUE(expression) ? (void)0 : (void)(debug_log_location(), ABORT())

#endif /*DEBUG_H*/
