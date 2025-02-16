/* What is the Ideal parser and lexer interface look like in C */

#define ALLOCATOR_IMPLEMENTATION
#include <stdio.h>

#define FORMAT_IMPLEMENTATION
#include "format.h"

#define TUI_IMPLEMENTATION

#define PUTCHAR_FUNCTION putchar
#include "allocator.h"
#include "dynvec.h"
#include "strutils.h"
#include "debug.h"

typedef enum {
    CHAR_TYPE_UNPRINTABLE,
    CHAR_TYPE_ALPHA,
    CHAR_TYPE_DIGIT,
    CHAR_TYPE_PUNCTUATION,
} CharType;

CharType char_type(char ch) {
    if((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
        return CHAR_TYPE_ALPHA;
    } else if (ch >= '0' && ch <= '9') {
        return CHAR_TYPE_DIGIT;
    } else if(ch >= 32 && ch <= 126) {
        return CHAR_TYPE_PUNCTUATION;
    } else {
        return CHAR_TYPE_UNPRINTABLE;
    }
}

typedef struct {
    CharType tag;
    char * value;
} Token;

typedef DynVec(Token) TokenVec;

TokenVec tokenize(Allocator a, char * input) {
    TokenVec result = {0};
    int i = 0;

    if(input == NULL) {
        return result;
    }

    for(i = 0; input[i] != 0; ++i ) {
        char ch = input[i];
        CharType tag = char_type(ch);
        char tok[2048] = {0}; 
        const int start = i;
        tok[i - start] = ch;

        while(tag != CHAR_TYPE_PUNCTUATION && char_type(input[i + 1]) == tag) {
            ++i;
            tok[i - start] = input[i];
        }

        {
            Token t = {0};
            t.tag = tag;
            t.value = string_duplicate(a, tok);
            dynvec_push(a, result, t);
        }
    }
    return result;
}


/*
int main() {
    char * src = "[print \"hi\"][variable count 1]\n[set count 20]";
    Allocator a = libc_allocator();
    TokenVec toks = tokenize(a, src);
    int i = 0;

    for(i = 0; i < toks.len; ++i) {
        const Token tok = toks.items[i];
        if(tok.tag == CHAR_TYPE_DIGIT) {
            printf("DIGIT=%s\n", tok.value);
        } else if(tok.tag == CHAR_TYPE_ALPHA) {
            printf("ALPHA=%s\n", tok.value);
        } else if(tok.tag == CHAR_TYPE_PUNCTUATION) {
            printf("PUNCT=%s\n", tok.value);
        } else {
            printf("ERROR\n");
        }
    }
}
*/
