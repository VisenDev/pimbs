



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



typedef struct {
    Allocator a;
    char * input;
    char * next;
    int i;
} Tokenizer;

Tokenizer tokenizer_init(Allocator a, char * input) {
    Tokenizer self = {0};
    self.a = a;
    self.input = string_duplicate(a, input);
    self.i = 0;
    self.next = NULL;
    return self;
}

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
    } else if(ch >= 33 && ch <= 126) {
        return CHAR_TYPE_PUNCTUATION;
    } else {
        return CHAR_TYPE_UNPRINTABLE;
    }
}

char peek_char(Tokenizer * self) {
    return self->input[self->i];
}

char get_char(Tokenizer * self) {
    char ch = self->input[self->i]; 
    ++self->i;
    return ch;
}


char * get_token(Tokenizer * self) {
    if(self->next != NULL) {
        char * tok = self->next;
        self->next = NULL;
        return tok;
    } else {
        char tok[2048] = {0}; 
        int count = 0;
        char ch = get_char(self);
        tok[count] = ch;
        CharType chtype = char_type(ch);

        if(ch == 0) {
            return NULL;
        }

        if(chtype == CHAR_TYPE_PUNCTUATION) {
            return string_duplicate(self->a, tok);
        }

        while(char_type(peek_char(self)) == chtype) {
            ch = get_char(self);
            ++count;
            tok[count] = ch;
        }

        /*
        while(char_type(ch) == chtype && ch != 0) {
            tok[count] = ch;
            ch = get_char(self);
            ++count;
        } 
        */

        /*printf("internal tok=%s\n", tok);*/
        return string_duplicate(self->a, tok);
    }
}


char * peek(Tokenizer * self) {

    /*This function should never be called twice in a row*/
    inline_assert(self->next == NULL);
    {
        char * tok = get_token(self);
        self->next = tok;
        return tok;
    }
}

int main() {
    char * src = "[print \"hi\"][variable count 1][set count 20]";
    Allocator a = libc_allocator();
    Tokenizer t = tokenizer_init(a, src);
    char * tok = get_token(&t);
    /*printf("copied input=%s\n", t.input);*/
    
    for(; tok != NULL; tok = get_token(&t)) {
        printf("Token=%s\n", tok); 
    }

    return 0;


}
