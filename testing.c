#include "optional.h"

typedef struct {
   char* name;
   int passed;
   int failed;
} testing_TestTracker;

typedef Optional(testing_TestTracker) testing_OptionalTestTracker;

typedef struct {
    testing_TestTracker overall;
    testing_OptionalTestTracker active;
} testing_State;

void testing_print_summary(const testing_State state) {
    printf("===============%s================\n", state.name);
    printf("PASSED: %i\n", state.passed);
    printf("FAILED: %i\n", state.failed);
    printf("================================\n\n");
}

void testing_start_test(testing_State * state, char* test_name) {
    //add data from last test to overall
    state->overall.passed += state.active.passed;
    state->overall.passed += state.active.passed;

    if(state->active.valid) {
        testing_print_summary(state->active.value);
    }

    //update active
    state->active.valid = true;
    state->active.value = {};
    state->active.value.name = test_name;

    printf("===============STARTING=%s================\n", state->active.value.name);
}

void testing_expect(testing_TestTracker tracker, const bool condition, const char description[]) {
    
}
