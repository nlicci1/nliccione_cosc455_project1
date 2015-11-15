#ifndef LEXEMES_H
#define LEXEMES_H

#include "main.h"

#define BOLD 0

typedef enum
{
    DOC_LEXEME = '#',
    VAR_LEXEME = '$',
    HEAD = '^',
    TITLEB = '<',
    TITLEE = '>',
    PARAB = '{',
    PARAE = '}',
    EQSIGN = '=',
    ITALICS = '*',
    LISTITEMB = '+',
    LISTITEME = ';',
    NEWLINE = '~',
    LINKB = '[',
    LINKE = ']',
    AUDIO = '@',
    VIDEO = '%',
    ADDRESSB = '(',
    ADDRESSE = ')'
} lexeme_chars_t;

// Array of lexeme characters
// LEXEME_SPECIAL_CHAR_CNT is the size of lexeme_char_arr
#define LEXEME_SPECIAL_CHAR_CNT 18
extern lexeme_chars_t lexeme_char_arr[];

// The following lexems strings need to be in order such that
// the string length is in an ascending order. This means that the smallest
// string values will be contained in the lower indexes and as the index value increases
// so will the string length. 

// These macros are to be used with LEXEME_DOC_STRINGS to directly access each individual lexeme
#define LEXEME_DOCE_IDX 0 
#define LEXEME_DOCB_IDX 1 
#define LEXEME_DOC_ARRAY_LEN 2
extern value_str_t LEXEME_DOC_STRINGS[];

// These macros are to be used with LEXEME_VAR_STRINGS to directly access each individual lexeme
#define LEXEME_DEFB_IDX 0
#define LEXEME_DEFUSEE_IDX 1
#define LEXEME_USEB_IDX 2
#define LEXEME_VAR_ARRAY_LEN 3
extern value_str_t LEXEME_VAR_STRINGS[];

// Call this function to see if the str passed in starts with a special
// lexeme character.
// Returns:
// TRUE - If only plain text is in the first element of the str
// FALSE - If the first char in str is a special syntax character
bool istext(char *str);

#endif 
