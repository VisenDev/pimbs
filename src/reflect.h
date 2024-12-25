#ifndef REFLECT_H
#define REFLECT_H

//#include <string.h>
// Add another layer of indirection to force macro expansion
#define NUM_ARGS_EXPAND(...) NUM_ARGS_INTERNAL(__VA_ARGS__)

// Correctly define NUM_ARGS_INTERNAL
#define NUM_ARGS_INTERNAL( \
        X100, X99, X98, X97, X96, X95, X94, X93, X92, X91, X90, X89, X88, X87, X86, X85, X84, X83, X82, X81, \
        X80, X79, X78, X77, X76, X75, X74, X73, X72, X71, X70, X69, X68, X67, X66, X65, X64, X63, X62, X61,  \
        X60, X59, X58, X57, X56, X55, X54, X53, X52, X51, X50, X49, X48, X47, X46, X45, X44, X43, X42, X41,  \
        X40, X39, X38, X37, X36, X35, X34, X33, X32, X31, X30, X29, X28, X27, X26, X25, X24, X23, X22, X21, \
        X20, X19, X18, X17, X16, X15, X14, X13, X12, X11, X10, X9, X8, X7, X6, X5, X4, X3, X2, X1, N, ...)  N

// Update NUM_ARGS to use the indirection macro
#define NUM_ARGS(...) \
   NUM_ARGS_EXPAND(__VA_ARGS__, \
           100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, 84, 83, 82, 81, 80, 79, 78, 77,  \
           76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66, 65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53,   \
           52, 51, 50, 49, 48, 47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29,   \
           28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3,  \
           2, 1)


#define REFLECTIVE_ENUM(enum_name, ...) \
    typedef enum { \
        __VA_ARGS__ \
    } enum_name; \
    \
    const char * enum_name ## _fields_string = { \
       #__VA_ARGS__ \
    }; \
    const unsigned int enum_name ## _fields_count = NUM_ARGS(__VA_ARGS__)
    //char * enum_name ## [NUM_ARGS(__VA_ARGS__)] = {0}


typedef struct {
    char * fields;
    unsigned int field_count;
} EnumMetadata;

//static inline char ** extract_enum_metadata(const char * const fields) {
//    const char * starting_token = strtok(fields, ",");
//}

//REFLECTIVE_ENUM(test, one, two, three);
#define REMOVE_PARENS(...) REMOVE_PARENS_INTERNAL(__VA_ARGS__)
#define REMOVE_PARENS_INTERNAL(arg) REMOVE_PARENS_HELPER arg
#define REMOVE_PARENS_HELPER(a, ...) a __VA_ARGS__

#define REFLECTIVE_STRUCT(struct_name, ...) \
    typedef struct __VA_ARGS__ struct_name; \
    const char * struct_name_metadata = #__VA_ARGS__

//REFLECTIVE_STRUCT(test,
//    {
//        int foo;
//        char bar;
//        bool biff;
//    });

#define TYPEDEF(name, ...) \
    typedef __VA_ARGS__ name; \
    const char * name ## _metadata = #__VA_ARGS__

TYPEDEF(Bazz,
    struct {
        int i;
        char foo;
        float biff;
    });

TYPEDEF( Foo,
    enum {
        FOO_ONE,
        FOO_TWO,
        FOO_THREE,
    });


#endif //REFLECT_H
