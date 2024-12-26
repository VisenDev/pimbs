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
#include "attributes.h"
#include "strutils.h"
#include "allocator.h"

#ifdef REFLECT_IMPLEMENTATION
    #define STB_C_LEXER_IMPLEMENTATION
    #include "3rdparty/stb_c_lexer.h"
#endif

struct TypeMetadata;

typedef enum {
    TYPE_PRIMITIVE,
    TYPE_STRUCT,
    TYPE_UNION,
} TypeTag;

#define VEC_TYPE struct TypeMetadata *
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

typedef union TypeValue {
    StructMetadata struct_;  //underscore used because "struct" as a field name won't compile
    UnionMetadata union_; 
    PrimitiveMetadata primitive_; 
} TypeValue;


typedef struct TypeMetadata {
    String name;
    TypeTag tag;
    TypeValue value;
} TypeMetadata;


#define HASH_TYPE struct TypeMetadata
#define HASH_NAME TypeMetadataTable
#ifdef REFLECT_IMPLEMENTATION
    #define HASH_IMPLEMENTATION
#endif
#include "hash.h"

typedef struct {
    TypeMetadataTable types;
} TypeRegistry;

NODISCARD PURE_FUNCTION
TypeRegistry TypeRegistry_init(void) 
#ifdef REFLECT_IMPLEMENTATION
{
    return (TypeRegistry){.types = TypeMetadataTable_init(),};
}
#else
;
#endif

void TypeRegistry_free(Allocator a, TypeRegistry * self)
#ifdef REFLECT_IMPLEMENTATION
{
    TypeMetadataTable_free(a, &self->types);
}
#else
;
#endif


NODISCARD
int record_type_metadata(Allocator a, TypeRegistry * registry, char * type_def, const unsigned long type_def_len) 
#ifdef REFLECT_IMPLEMENTATION
{

    (void) registry;
    const unsigned int buflen = 10000;
    char * buf = a.alloc(a, buflen);
    if(buf == NULL) {
        return ERR_ALLOCATION_FAILURE;
    }

    stb_lexer lex;
    stb_c_lexer_init(&lex, type_def, type_def + type_def_len, buf, buflen);
    while (stb_c_lexer_get_token(&lex)) {
       if (lex.token == CLEX_parse_error) {
          printf("\n<<<PARSE ERROR>>>\n");
          break;
       }
    //   print_token(&lex);
       printf("  ");
    }

    return ERR_NONE;
}
#else
;
#endif


#endif //REFLECT_H
