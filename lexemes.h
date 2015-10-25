#ifndef LEXEMES_H
#define LEXEMES_H

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

// These macros are to be used with LEXEME_STRINGS to directly access each individual lexeme
#define LEXEME_DOCB_IDX 0
#define LEXEME_DOCE_IDX 1
#define LEXEME_DEFB_IDX 2
#define LEXEME_DEFUSEE_IDX 3
#define LEXEME_USEB_IDX 4
#define LEXEME_BOLD_TEXT_IDX 5
#define LEXEME_STRING_ARRAY_LEN 6
extern value_str_t LEXEME_STRINGS[];

#endif 
