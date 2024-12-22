#ifndef TESTING_H
#define TESTING_H

typedef struct {
   const char* name;
   int passed;
   int failed;
} TestingTracker;

typedef struct {
    TestingTracker overall;
    TestingTracker active;
} TestingState;

TestingState testing_init();
void testing_deinit(TestingState * state);
void testing_start_test(TestingState * state, const char* test_name);
#define testing_expect(state, condition) testing_expect_internal(state, condition, __LINE__, __FILE__)
void testing_expect_internal(TestingState*, const bool, const int, const char *);

#endif //TESTING_H
