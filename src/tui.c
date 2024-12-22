#include <stdio.h>
#include <stdarg.h>
#include "tui.h"

void tui_printf(const char * format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args); 
    printf(TUI_RESET);
    va_end(args);
}
