#if !defined(TESTING_H) \
    /*|| (defined(TESTING_IMPLEMENTATION) && !defined(TESTING_IMPLEMENTED))*/

#define TESTING_H
#ifdef TESTING_IMPLEMENTATION
    #define TESTING_IMPLEMENTED
#endif

#include "tui.h"
#include "strutils.h"
#include "debug.h"

typedef struct {
   const char* name;
   int passed;
   int failed;
} TestingTracker;

typedef struct {
    TestingTracker overall;
    TestingTracker active;
} TestingState;

static unsigned int padding_needed(const char * const str, const unsigned int desired_width)
#ifdef TESTING_IMPLEMENTATION
{
    const unsigned int len = (unsigned int)string_length(str, 40);
    return desired_width - len;
}
#else
;
#endif

static void testing_print_header(const char * str)
#ifdef TESTING_IMPLEMENTATION
{
    const unsigned int padding = padding_needed(str, 40);
    const unsigned int pre_padding = padding / 2;
    const unsigned int post_padding = padding - pre_padding;
    unsigned int i = 0;

    for(i = 0; i < pre_padding; ++i) {
        tui_put_str("=");
    }
    tui_put_str(str);
    for(i = 0; i < post_padding; ++i) {
        tui_put_str("=");
    }
    tui_put_str("\n");
}
#else
;
#endif

static void testing_print_summary(const TestingTracker tracker)
#ifdef TESTING_IMPLEMENTATION
{
    testing_print_header(tracker.name);
    tui_put_str_reset(TUI_GREEN"PASSED: ");
    tui_put_long(tracker.passed);
    tui_put_str("\n");
    tui_put_str_reset(TUI_RED"FAILED: ");
    tui_put_long(tracker.failed);
    tui_put_str("\n");
    testing_print_header("");
}
#else
;
#endif


static TestingState testing_init(void)
#ifdef TESTING_IMPLEMENTATION
{
    TestingTracker overall = {0};
    TestingTracker active = {0};
    TestingState result = {0};

    overall.name = "SUMMARY";
    result.overall = overall;
    result.active = active;
    return result;
}
#else
;
#endif

static void testing_update_overall(TestingState * state)
#ifdef TESTING_IMPLEMENTATION
{

    if(state->active.name != NULL) {
        if(state->active.failed != 0) {
            tui_put_str_reset(TUI_RED  "[failed...]\n"); 
        } else {
            tui_put_str_reset(TUI_GREEN"[completed]\n"); 
        }
        state->overall.passed += state->active.passed;
        state->overall.failed += state->active.failed;
    }
}
#else
;
#endif

static void testing_deinit(TestingState * state)
#ifdef TESTING_IMPLEMENTATION
{

    testing_update_overall(state);
    testing_print_summary(state->overall);
}
#else
;
#endif


static void testing_start_test(TestingState * state, const char* test_name)
#ifdef TESTING_IMPLEMENTATION
{
    testing_update_overall(state);

    /*update active*/
    {
        const TestingTracker active = {0};
        state->active = active;
        state->active.name = test_name;
    }

    /*print padding*/
    {
        const int characters = tui_put_str("testing.") + tui_put_str(state->active.name) + tui_put_str("..."); 
        int i = 0;
        for(i = 0; i < 40 - characters; ++i) {
            tui_put_str(" ");
        }
    }
}
#else
;
#endif

#define testing_expect(state, condition) testing_expect_internal(state, condition, __LINE__, __FILE__)
static void testing_expect_internal(TestingState * state, const int condition, const int line, const char * file)
#ifdef TESTING_IMPLEMENTATION
{
    if(condition) {
        state->active.passed += 1;
    } else {
        tui_put_str("\n");
        tui_put_str(TUI_RED);
        {
            const int characters = debug_log_location();
            int i = 0;
            tui_put_str(TUI_RESET);
            for(i = 0; i < 40 - characters; ++i) {
                tui_put_str(" ");
            }
            state->active.failed += 1;
        }
    }
}
#else
;
#endif

#endif /*TESTING_H*/
