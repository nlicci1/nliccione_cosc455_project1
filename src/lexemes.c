#include "main.h"
#include "lexemes.h"

lexeme_chars_t lexeme_char_arr[] = 
{    
    DOC_LEXEME,
    VAR_LEXEME,
    HEAD,
    TITLEB,
    TITLEE,
    PARAB,
    PARAE,
    EQSIGN,
    ITALICS,
    LISTITEMB,
    LISTITEME,
    NEWLINE,
    LINKB,
    LINKE,
    AUDIO,
    VIDEO,
    ADDRESSB,
    ADDRESSE
};

value_str_t LEXEME_DOC_STRINGS[] = 
{
    { LEXEME_DOCE_IDX, "#END" },
    { LEXEME_DOCB_IDX, "#BEGIN" }
};

value_str_t LEXEME_VAR_STRINGS[] = 
{
    { LEXEME_DEFB_IDX, "$DEF" },
    { LEXEME_DEFUSEE_IDX, "$END" },
    { LEXEME_USEB_IDX, "$USE" }, 
};

bool istext(char *str)
{
    bool retval = TRUE;

    if (!str)
    {
        return FALSE;
    }

    lexeme_chars_t spec_char;
    int i;
   
    for (i = 0; i < LEXEME_SPECIAL_CHAR_CNT; i++)
    {
        spec_char = lexeme_char_arr[i];
        
        if (str[0] == spec_char)
        {
            retval = FALSE;
            break;
        }
    }
    
    return retval;
}
