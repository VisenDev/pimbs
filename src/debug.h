#ifndef DEBUG_H
#define DEBUG_H

//#ifndef TUI_H
//#error "\"tui.h\" must be included before \"debug.h\""
//#endif //TUI_H
#include "tui.h"
#include "attributes.h"


#define int_specifier "%d"
#define unsigned_int_specifier "%u"
#define short_specifier "%hd"
#define unsigned_short_specifier "%hu"
#define long_specifier "%ld"
#define unsigned_long_specifier "%lu"
#define long_long_specifier "%lld"
#define unsigned_long_long_specifier "%llu"
#define float_specifier "%f"
#define double_specifier "%f"
#define char_specifier "%c"
#define unsigned_char_specifier "%hhu"
#define signed_char_specifier "%hhd"
#define char_pointer_specifier "%s"
#define void_pointer_specifier "%p"
#define int8_t_specifier "%hhd"
#define uint8_t_specifier "%hhu"
#define int16_t_specifier "%hd"
#define uint16_t_specifier "%hu"
#define int32_t_specifier "%d"
#define uint32_t_specifier "%u"
#define int64_t_specifier "%lld"
#define uint64_t_specifier "%llu"
#define size_t_specifier "%zu"
#define ptrdiff_t_specifier "%td"


//#define debug_printf_info(format, ...)
//#define debug_printf_warn(format, ...)
//#define debug_printf_error(format, ...)

#define log_location() tui_printf("   File: %s, Function: %s, Line: %d\n", __FILE__, __func__, __LINE__)
       
#define debug_assert(type, lhs, operator, rhs) \
    do { \
        if(LIKELY_FALSE(!((lhs) operator (rhs)))) { \
            tui_printf(TUI_RED   "\nASSERTION FAILURE: %s %s %s\n", #lhs, #operator, #rhs); \
            tui_printf(TUI_YELLOW"   lhs expression %s expands to " type ## _specifier "\n", #lhs, lhs); \
            tui_printf(TUI_YELLOW"   rhs expression %s expands to " type ## _specifier "\n", #rhs, rhs); \
            log_location(); \
            tui_printf(TUI_RED   "   ABORTING...\n\n"); \
            ABORT(); \
        } \
    } while (0)

#define simple_assert(expression, message) \
    do { \
        const int condition = expression; \
        if(LIKELY_FALSE(!condition)) { \
            tui_printf(TUI_RED   "\nASSERTION FAILURE: %s\n", #expression); \
            tui_printf(TUI_YELLOW"   %s\n", message); \
            log_location(); \
            tui_printf(TUI_RED   "   ABORTING...\n\n"); \
            ABORT(); \
        } \
    } while (0)



#endif //DEBUG_H
