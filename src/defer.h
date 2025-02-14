#ifndef DEFER_H
#define DEFER_H

#define defer(label) \
    while(0) \
        while(1) \
            if (1) { \
                goto label##_done_; \
            } else label:

#define deferred(label) do { goto label; label##_done_:; } while (0)

#endif
