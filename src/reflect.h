#ifndef REFLECT_H
#define REFLECT_H

#define REFLECTIVE_TYPEDEF(name, ...) \
    typedef __VA_ARGS__ name; \
    const char * name ## _metadata = #__VA_ARGS__

REFLECTIVE_TYPEDEF(Bazz,
    struct {
        int i;
        char foo;
        float biff;
    });

REFLECTIVE_TYPEDEF( Foo,
    enum {
        FOO_ONE,
        FOO_TWO,
        FOO_THREE,
    });


//IMPLEMENTATION
#include "strutils.h"
#include "allocator.h"

#ifdef REFLECT_IMPLEMENTATION
    #define STB_C_LEXER_IMPLEMENTATION
    #include "3rdparty/stb_c_lexer.h"
#endif

struct TypeMetadata;

#define VEC_TYPE struct TypeMetadata
#define VEC_NAME TypeMetadataVec
#ifdef REFLECT_IMPLEMENTATION
    #define VEC_IMPLEMENTATION
#endif
#include "vec.h"

typedef struct {
    TypeMetadataVec fields;
} StructMetadata;

typedef struct {
    TypeMetadataVec fields;
} UnionMetadata;

typedef struct {
    String format_specifier;
} PrimitiveMetadata;

typedef union {
    StructMetadata struct_;  //underscore used because "struct" as a field name won't compile
    UnionMetadata union_; 
    PrimitiveMetadata primitive_; 
} TypeValue;

typedef enum {
    TYPE_PRIMITIVE,
    TYPE_STRUCT,
    TYPE_UNION,
} TypeTag;

typedef struct TypeMetadata {
    String name;
    TypeTag tag;
    TypeValue value;
} TypeMetadata;


TypeMetadata * parse_type_metadata(Allocator a, const char * const type_def, const unsigned long type_def_len) 
#ifdef REFLECT_IMPLEMENTATION
{
    TypeMetadata * result = a.alloc(sizeof(TypeMetadata));
    if(result == NULL) {
        return NULL;
    }

    const unsigned int buflen = 10000;
    char * buf = a.alloc(buflen);
    if(buf == NULL) {
        return NULL;
    }

    stb_lexer lex;
    stb_c_lexer_init(&lex, type_def, type_def + type_def_len, buf, buflen);
    while (stb_c_lexer_get_token(&lex)) {
       if (lex.token == CLEX_parse_error) {
          printf("\n<<<PARSE ERROR>>>\n");
          break;
       }
       print_token(&lex);
       printf("  ");
    }
}
#else
;
#endif


#endif //REFLECT_H
