#ifndef ERRORS_H
#define ERRORS_H

#include "assert.h"

#define ERR_NONE                0
#define ERR_ALLOCATION_FAILURE  1
#define ERR_INDEX_OUT_OF_BOUNDS 2

static inline char const * error_name(const int error_code) {
    switch(error_code) {
        case ERR_NONE: 
            return "ERR_NONE";
        case ERR_ALLOCATION_FAILURE:
            return "ERR_ALLOCATION_FAILURE";
        case ERR_INDEX_OUT_OF_BOUNDS:
            return "ERR_INDEX_OUT_OF_BOUNDS";
        }
    }

    debug_assert(char *, "Invalid error code", ==, 0);
}


#endif //ERRORS_H
