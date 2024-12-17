#define PIMBS_IMPLEMENTATION
#include "pimbs.h"

int main() {
    pimbs_testing_State * t = pimbs_testing_init();
    {
        pimbs_vector_run_tests(t);
        pimbs_ss_run_tests(t);
        pimbs_generic_run_tests(t);
    }
    pimbs_testing_deinit(t);
}
