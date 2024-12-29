#ifndef DEFER_H
#define DEFER_H

/* label##_i has to be declared before the for loop because of C89 */
#define defer(label) \
    int label##_i; \
    while(0) label: \
        for(label##_i = 0;; ++label##_i) \
            if(label##_i > 0) \
                goto label##_done; \
            else 

#define deferred(label) do { goto label; label##_done:; } while (0)

#endif
