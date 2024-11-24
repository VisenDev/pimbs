#include <vector.h>
#include <stdint.h>

static uint64_t hash(unsigned char *str)
{
    uint64_t hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

typedef struct
    //DATA
    char* data;
    uint64_t data_len;
    uint64_t data_capacity;

    #define MAX_KEYS 32
    typedef Vector String;

    //KEYS
    String keys[MAX_KEYS];

    #define HASH_CAPACITY 32
    uint64_t  
} Object;
