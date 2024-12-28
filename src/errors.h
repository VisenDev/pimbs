#ifndef ERRORS_H
#define ERRORS_H

#include "debug.h"

#define ERR_NONE                0
#define ERR_ALLOCATION_FAILURE  1
#define ERR_INDEX_OUT_OF_BOUNDS 2
#define ERR_NOT_FOUND           3

static char const * error_name(const int error_code) {
    switch(error_code) {
        case ERR_NONE: 
            return "ERR_NONE";
        case ERR_ALLOCATION_FAILURE:
            return "ERR_ALLOCATION_FAILURE";
        case ERR_INDEX_OUT_OF_BOUNDS:
            return "ERR_INDEX_OUT_OF_BOUNDS";
        case ERR_NOT_FOUND:
            return "ERR_NOT_FOUND";
        default:
            simple_assert(0, "Invalid error code");
    }

    return NULL;
}


#endif /*ERRORS_H*/
