#ifndef TESTING_H
#define TESTING_H

#include <stdio.h>
#include "color.h"
#include <string.h>
#include <stdint.h>

typedef struct {
   const char* name;
   int passed;
   int failed;
} testing_TestTracker;

typedef struct {
    testing_TestTracker overall;
    testing_TestTracker active;
} testing_State;

#endif //TESTING_H


//function body implementation macro
#undef implementation

#ifdef TESTING_IMPLEMENTATION
   #define implementation(...) {__VA_ARGS__}
#else
   #define implementation(...) ;
#endif


testing_State testing_start() implementation (
    return (testing_State) {
        .overall = (testing_TestTracker){.name = "SUMMARY"},
        .active = {0},
    };
)

static void testing_print_header(const char * str) implementation (
    const size_t desired_width = 40;
    const size_t len = strlen(str);
    const size_t padding_needed = desired_width - len;
    const size_t pre_padding = padding_needed / 2;
    const size_t post_padding = padding_needed - pre_padding;

    for(uint64_t i = 0; i < pre_padding; ++i) {
        printf("=");
    }
    printf("%s", str);
    for(uint64_t i = 0; i < post_padding; ++i) {
        printf("=");
    }
    printf("\n");
)

static void testing_print_summary(const testing_TestTracker tracker) implementation (
    testing_print_header(tracker.name);
    printf("PASSED: " ANSI_COLOR_GREEN "%i\n" ANSI_COLOR_RESET, tracker.passed);
    printf("FAILED: " ANSI_COLOR_RED "%i\n" ANSI_COLOR_RESET, tracker.failed);
    testing_print_header("");
)

void testing_end(testing_State * state) implementation (
    if(state->active.name != NULL) {
        state->overall.passed += state->active.passed;
        state->overall.failed += state->active.failed;
    }
    testing_print_summary(state->overall);
    if(state->overall.failed != 0) {
        exit(1);
    } else {
        exit(0);
    }
)


void testing_start_test(testing_State * state, const char* test_name) implementation (
    if(state->active.name != NULL) {
        state->overall.passed += state->active.passed;
        state->overall.failed += state->active.failed;
    }

    //update active
    state->active = (testing_TestTracker){0};
    state->active.name = test_name;
)

#define testing_expect(state, condition, name) _testing_expect(state, condition, name, __LINE__, __FILE__)
void _testing_expect(
        testing_state * state,
        const bool condition,
        const char name[],
        const int line,
        const char * file
) implementation (
    if(condition) {
        printf("./testing %s.%s... " ANSI_COLOR_GREEN "[PASSED]\n" ANSI_COLOR_RESET, state->active.name, name); 
        state->active.passed += 1;
    } else {
        printf("./testing %s.%s... " ANSI_COLOR_RED "[FAILED]\n" ANSI_COLOR_RESET, state->active.name, name); 
        printf(ANSI_COLOR_RED "   -> %s:%i\n" ANSI_COLOR_RESET , file , line); 
        state->active.failed += 1;
    }
)

