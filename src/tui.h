#ifndef TUI_H
#define TUI_H

#define TUI_RED     "\x1b[31m"
#define TUI_GREEN   "\x1b[32m"
#define TUI_YELLOW  "\x1b[33m"
#define TUI_BLUE    "\x1b[34m"
#define TUI_MAGENTA "\x1b[35m"
#define TUI_CYAN    "\x1b[36m"
#define TUI_RESET   "\x1b[0m"

#define debug_printf(...) printf(__VA_ARGS__)
//#define debug_printf(...) 

void tui_printf(const char * format, ...);

#endif //TUI_H
