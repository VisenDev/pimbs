#if !defined(TUI_H) || (defined(TUI_IMPLEMENTATION) && !defined(TUI_IMPLEMENTED))
#define TUI_H
#ifdef TUI_IMPLEMENTATION
   #define TUI_IMPLEMENTED
#endif

#define TUI_RED     "\x1b[31m"
#define TUI_GREEN   "\x1b[32m"
#define TUI_YELLOW  "\x1b[33m"
#define TUI_BLUE    "\x1b[34m"
#define TUI_MAGENTA "\x1b[35m"
#define TUI_CYAN    "\x1b[36m"
#define TUI_RESET   "\x1b[0m"


#ifndef LOG_FUNCTION
    static void dummy_log_function(const char * format, ...)
    #ifdef TUI_IMPLEMENTATION
    {
        (void)format;
    }
    #else
    ;
    #endif
    #define LOG_FUNCTION(...) dummy_log_function(__VA_ARGS__)
#endif //LOG_FUNCTION
       

#define tui_printf(...) do { LOG_FUNCTION(__VA_ARGS__); LOG_FUNCTION(TUI_RESET); } while (0)
#define debug_printf tui_printf

#endif //TUI_H
