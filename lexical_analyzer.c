#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "main.h"
#include "lexical_analyzer.h"
#include "lexemes.h"

// Removes leading and trailing white space from 'str' and returns a copy of that string
// allocated dynamically. 
static char *trim(const char *str)
{
    char *retval = NULL;
    const char *end = NULL;
    unsigned int new_strlen = 0;
    
    if (str == NULL)
    {
        return retval;
    }

    end = str + strlen(str) - 1;

    // Trim leading ws
    while(isspace(*str))
    {
        str++;
    }

    if (*str != '\0')
    {
        // Trim trailing ws
        while (end > str && isspace(*end))
        {
            end--;
        }
        
        end++; 
       
        if ((new_strlen = end - str) > 0)
        {
            retval = (char *) malloc(new_strlen + 1);
            memset(retval, '\0', new_strlen + 1);
            strncpy(retval, str, new_strlen);
        }
    }
    
    return retval;
}

int get_token(FILE *source_file, char *token_buffer, unsigned int blen) 
{
    char *tmp_token = NULL;
    int i;
    int tmp_char = 0;
    unsigned int retval = 0;
    unsigned int tmp_len = 0;
    char token_char[2]; 
    char token;
    
    if (token_buffer == NULL || blen <= 0)
    {
        return 0;
    }

    memset(token_buffer, '\0', blen);

    while (retval == 0)
    {
        tmp_char = fgetc(source_file);

        if (tmp_char == EOF)
        {
            retval = EOF;
            break;
        }
        
        memset(token_char, '\0', 2);
        token = (char) tmp_char;
        token_char[0] = token;
        switch (token)
        {   
            // If we have one of these characters than all we need to do
            // is check to see if we have an empty string. If we do we return 
            // the char as the current token other wise we return the string in the 
            // buffer. 
            case TITLEB:
            case TITLEE:
            case PARAB:
            case PARAE:
            case EQSIGN:
            case LISTITEMB:
            case LISTITEME:
            case NEWLINE:
            case LINKB:
            case LINKE:
            case AUDIO:
            case VIDEO:
            case ADDRESSB:
            case ADDRESSE:
                if (strlen(token_buffer) > 0)
                {
                    ungetc((int) token, source_file); }
                else
                {
                    strcat(token_buffer, token_char);
                }
                    
                retval = LEXICAL_PARSE_SUCCESS;
                break;
            case ITALICS:
                if (strlen(token_buffer) > 0)
                {
                    ungetc((int) token, source_file);
                    retval = LEXICAL_PARSE_SUCCESS;
                }
                else
                {
                    // Add the '*' character into our buffer and then
                    // check to see if the next char from the stream is also
                    // a '*'. If it is append it, if not put it back into the stream
                    // and exit this function. 
                    strcat(token_buffer, token_char);
                    
                    tmp_char = fgetc(source_file);
                    if (tmp_char == EOF)
                    {
                        retval = EOF;
                    }
                    else
                    {
                        memset(token_char, '\0', 2);
                        token = (char) tmp_char;
                        token_char[0] = token;
                    
                        if (token == ITALICS)
                        {
                            strcat(token_buffer, token_char);
                        }
                        else
                        {
                            ungetc((int) token, source_file);
                        }

                        retval = LEXICAL_PARSE_SUCCESS;
                    }
                }
                break;
            case DOC_LEXEME:
                retval = LEXICAL_PARSE_ERROR;
                break;
            case VAR_LEXEME:
                if (strlen(token_buffer) > 0)
                {
                    ungetc((int) token, source_file);
                    retval = LEXICAL_PARSE_SUCCESS;
                }
                else
                {
                    strcat(token_buffer, token_char);
                    /*
                    // value_str_t LEXEME_STRINGS[] 
                    // LEXEME_STRING_ARRAY_LEN 
                    // for (i = 0; i < LEXEME_STRING_ARRAY_LEN; i++)
                    int fgetpos(FILE *stream, fpos_t *pos);
                    int fsetpos(FILE *stream, fpos_t *pos);
              
                    */
                }
                    
                retval = LEXICAL_PARSE_SUCCESS;
                break;
            default:
                if (strlen(token_buffer) >= blen - 1)
                {
                    fprintf(stderr, "Error: Token %s to large\n", token_buffer);
                    memset(token_buffer, '\0', blen);
                    retval = LEXICAL_PARSE_ERROR;
                }
                else
                {
                    strcat(token_buffer, token_char);
                }
                break;
        }
    }

    return retval;
}










