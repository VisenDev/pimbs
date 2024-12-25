#if !defined(TESTING_H) \
    || (defined(TESTING_IMPLEMENTATION) && !defined(TESTING_IMPLEMENTED))

#define TESTING_H
#ifdef TESTING_IMPLEMENTATION
    #define TESTING_IMPLEMENTED

    #ifndef TUI_H
        #error "\"tui.h\" must be included before \"testing.h\" is implemented"
    #endif
#endif


typedef struct {
   const char* name;
   int passed;
   int failed;
} TestingTracker;

typedef struct {
    TestingTracker overall;
    TestingTracker active;
} TestingState;

//TODO remove string.h dependency
#include <string.h>

static void testing_print_header(const char * str)
#ifdef TESTING_IMPLEMENTATION
{
    const unsigned int desired_width = 40;
    const unsigned int len = (unsigned int)strnlen(str, 40);
    const unsigned int padding_needed = desired_width - len;
    const unsigned int pre_padding = padding_needed / 2;
    const unsigned int post_padding = padding_needed - pre_padding;

    for(unsigned int i = 0; i < pre_padding; ++i) {
        tui_printf("=");
    }
    tui_printf("%s", str);
    for(unsigned int i = 0; i < post_padding; ++i) {
        tui_printf("=");
    }
    tui_printf("\n");
}
#else
;
#endif

static void testing_print_summary(const TestingTracker tracker)
#ifdef TESTING_IMPLEMENTATION
{
    testing_print_header(tracker.name);
    tui_printf(TUI_GREEN"PASSED: %i\n",  tracker.passed);
    tui_printf(TUI_RED"FAILED: %i\n", tracker.failed);
    testing_print_header("");
}
#else
;
#endif


TestingState testing_init(void)
#ifdef TESTING_IMPLEMENTATION
{
    return (TestingState) {
        .overall = (TestingTracker){.name = "SUMMARY"},
        .active = {0},
    };
}
#else
;
#endif


void testing_deinit(TestingState * state)
#ifdef TESTING_IMPLEMENTATION
{
    if(state->active.name != NULL) {
        state->overall.passed += state->active.passed;
        state->overall.failed += state->active.failed;
    }
    testing_print_summary(state->overall);
}
#else
;
#endif


void testing_start_test(TestingState * state, const char* test_name)
#ifdef TESTING_IMPLEMENTATION
{
    if(state->active.name != NULL) {
        state->overall.passed += state->active.passed;
        state->overall.failed += state->active.failed;
    }

    //update active
    state->active = (TestingTracker){0};
    state->active.name = test_name;
}
#else
;
#endif

#define testing_expect(state, condition) testing_expect_internal(state, condition, __LINE__, __FILE__)
void testing_expect_internal(TestingState * state, const int condition, const int line, const char * file)
#ifdef TESTING_IMPLEMENTATION
{
    if(condition) {
        tui_printf("testing.%s.%d... ", state->active.name, state->active.passed + state->active.failed); 
        tui_printf(TUI_GREEN"[PASSED]\n");
        state->active.passed += 1;
    } else {
        tui_printf("./testing.%s.%d... ", state->active.name, state->active.passed + state->active.failed); 
        tui_printf(TUI_RED"[FAILED]\n");
        tui_printf(TUI_RED"   -> %s:%i\n", file , line); 
        state->active.failed += 1;
    }
}
#else
;
#endif

#endif //TESTING_H
