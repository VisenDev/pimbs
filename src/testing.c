#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include "tui.h"
#include "string.h"
#include "stdint.h"
#include "testing.h"

static void testing_print_header(const char * str) {
    const size_t desired_width = 40;
    const size_t len = strlen(str);
    const size_t padding_needed = desired_width - len;
    const size_t pre_padding = padding_needed / 2;
    const size_t post_padding = padding_needed - pre_padding;

    for(uint64_t i = 0; i < pre_padding; ++i) {
        tui_printf("=");
    }
    tui_printf("%s", str);
    for(unsigned int i = 0; i < post_padding; ++i) {
        tui_printf("=");
    }
    tui_printf("\n");
    fflush(stdout);
}

static void testing_print_summary(const TestingTracker tracker) {
    testing_print_header(tracker.name);
    tui_printf(TUI_GREEN"PASSED: %i\n",  tracker.passed);
    tui_printf(TUI_RED"FAILED: %i\n", tracker.failed);
    testing_print_header("");
}


TestingState testing_init() {
    return (TestingState) {
        .overall = (TestingTracker){.name = "SUMMARY"},
        .active = {0},
    };
}

void testing_deinit(TestingState * state) {
    if(state->active.name != NULL) {
        state->overall.passed += state->active.passed;
        state->overall.failed += state->active.failed;
    }
    testing_print_summary(state->overall);
}


void testing_start_test(TestingState * state, const char* test_name) {
    if(state->active.name != NULL) {
        state->overall.passed += state->active.passed;
        state->overall.failed += state->active.failed;
    }

    //update active
    state->active = (TestingTracker){0};
    state->active.name = test_name;
}

void testing_expect_internal(
        TestingState * state,
        const bool condition,
        const int line,
        const char * file
) {
    if(condition) {
        tui_printf("testing.%s.%d... ", state->active.name, state->active.passed + state->active.failed); 
        tui_printf(TUI_GREEN"[PASSED]\n");
        state->active.passed += 1;
    } else {
        printf("./testing.%s.%d... ", state->active.name, state->active.passed + state->active.failed); 
        tui_printf(TUI_RED"[FAILED]\n");
        printf(TUI_RED"   -> %s:%i\n", file , line); 
        state->active.failed += 1;
    }
}
