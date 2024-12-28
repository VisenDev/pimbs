#ifndef DEFER_H
#define DEFER_H

/*only used as a for loop counter becase C89 doesn't let you declare variables inside of a for loop */
/*static unsigned int defer_counter = 0;*/

/*
#define defer(label) \
    label: for(; defer_counter >= 1; --defer_counter ) \
    if ( defer_counter == 1 ) { \
        goto label ## _done;  \
    } else

#define deferred(label) \
    do {  \
        defer_counter = 2;  \
        goto label; \
        label ## _done:; \
        defer_counter = 0; \
    } while (0)

*/


/* label##_i has to be declared before the for loop because of C89 */
#define defer(label) \
    int label##_i; \
    if(0) label: \
        for(label##_i = 0;; ++label##_i) \
            if(label##_i > 0) \
                goto label##_done; \
            else 

#define deferred(label) do { goto label; label##_done:; } while (0)

#endif
