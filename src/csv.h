#ifndef CSV_H
#define CSV_H

#include "allocator.h"
#include "strutils.h"

typedef struct {
    unsigned long begin;
    unsigned long end;
} CsvBounds;

typedef struct CsvParser {
    char * const input;
    unsigned long input_len;

    /*csv parsing config*/
    char delimiter;
    char * delimiter_escape_sequence;

    char row_delimiter;
    char * row_delimiter_escape_sequence;

    int support_quoted_strings;
    int allow_delimiter_inside_quotes;
    int allow_row_delimiter_inside_quotes;
    char quote;
    char * quote_escape_sequence;

    int skip_whitespace;
    int require_equal_length_rows;

    /*csv lookup index*/
    unsigned long rows;
    unsigned long columns;
    CsvBounds index[][];

} CsvParser;


CsvParser csv_init_standard_config(void) {
    CsvParser self = {0};

    self.delimiter = ',';
    self.delimiter_escape_sequence = "\,";

    self.row_delimiter = '\n';
    self.row_delimiter_escape_sequence = NULL;

    self.support_quoted_strings = 1;
    self.allow_delimiter_inside_quotes = 1;
    self.allow_row_delimiter_inside_quotes = 1;
    self.quote = '\"';
    self.quote_escape_sequence = "\\\"";

    skip_whitespace = 1;
    require_equal_length_rows = 0;
}

unsigned long csv_count_rows(CsvParser * self) {
    unsigned long i = 0;
    unsigned long rows = 1;
    int check_escape = self.row_delimiter_escape_sequence != NULL;
    unsigned long escape_len = string_length(self->row_delimiter_escape_sequence, 4);

    if(self->input_len == 0 || self->input == NULL) {
        return 0;
    }

    for(i = 0; i < self->input_len; ++i) {
        if(check_escape) {
            if(string_equal(self->input[i], self->row_delimiter_escape_sequence, escape_len) {
                ++i; /*skip row delimiter*/
                continue;
            } 
        }
        if(self->input[i] == self->row_delimiter_escape_sequence) {
           rows += 1; 
        }
    }
    return rows;
}



int csv_parse(
        Allocator a,
        CsvParser * self,
        const char * const input,
        const unsigned long input_len) {
    self->input = input;
    self->input_len = input_len;

}






#endif /*CSV_H*/
