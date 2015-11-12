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

// This function gets a lexeme string from the file and copies it to the buffer argument.
// The possible lexeme strings are passed in as the parameter name lexeme_str_list. 
// If the next 'n' bytes in the file do not match any of the lexeme strings provided in the lexeme_str_list array
// then parse error is returned. Otherwise success is returned if a match is found and the next char (if one exists in the file)
// in the source file is a white space character.
static int get_lexeme_string(FILE *source_file, char *buffer, size_t blen, value_str_t *lexeme_str_list, size_t array_size)
{
    int tmp_char;
    int i;
    int retval;
    unsigned int tmp_buffer_len;
    unsigned int lexeme_strlen;
    
    if (buffer == NULL || blen <= 0 || lexeme_str_list == NULL || array_size <= 0)
    {
        return 0;
    }

    retval = LA_PARSE_ERROR;
    for (i = 0; i < array_size; i++)
    {
        tmp_buffer_len = strlen(buffer); 
        lexeme_strlen = strlen(lexeme_str_list[i].str);

        if (tmp_buffer_len < lexeme_strlen)
        {
            if (fgets(buffer + tmp_buffer_len, lexeme_strlen - tmp_buffer_len + 1, source_file) == NULL)
            {
                fprintf(stderr, "Error: Token %s is not a valid variable lexeme string (EOF reached)\n", buffer);
                break;
            }
            else if(strlen(buffer) != lexeme_strlen)
            {
                fprintf(stderr, "Error: Token %s is not a valid variable lexeme string\n", buffer);
                break;
            }
         }
         else if(tmp_buffer_len > lexeme_strlen)
         {
            fprintf(stderr, "Error: Token %s is not a valid variable lexeme string\n", buffer);
            break;
         }

         if (strncasecmp(buffer, lexeme_str_list[i].str, lexeme_strlen) == 0)
         {
            tmp_char = fgetc(source_file);
            if (isspace(tmp_char) || tmp_char == EOF)
            {
                retval = LA_PARSE_SUCCESS;
                break;
            }
            else
            {
                fprintf(stderr, "Error: Token %s is not a valid variable lexeme string\n", buffer);
            }
         }
     }

     return retval;

}

static int get_token(FILE *source_file, char *token_buffer, unsigned int blen) 
{
    int tmp_char = 0;
    unsigned int retval = 0;
    char token_char[2]; 
    char token;
    
    if (token_buffer == NULL || blen <= 0)
    {
        return 0;
    }

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
            case HEAD:
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
                    
                retval = LA_PARSE_SUCCESS;
                break;
            case ITALICS:
                if (strlen(token_buffer) > 0)
                {
                    ungetc((int) token, source_file);
                    retval = LA_PARSE_SUCCESS;
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

                        retval = LA_PARSE_SUCCESS;
                    }
                }
                break;
            case DOC_LEXEME:
                if (strlen(token_buffer) > 0)
                {
                    ungetc((int) token, source_file);
                    retval = LA_PARSE_SUCCESS;
                }
                else
                {
                    strcat(token_buffer, token_char);
                    retval = get_lexeme_string(source_file, token_buffer, blen, LEXEME_DOC_STRINGS, LEXEME_DOC_ARRAY_LEN);

                    if (retval != LA_PARSE_SUCCESS)
                    {
                        fprintf(stderr, "Error: Lexical token error: Invalid token %s\n", token_buffer);
                    }
                }
                break;
            case VAR_LEXEME:
                if (strlen(token_buffer) > 0)
                {
                    ungetc((int) token, source_file);
                    retval = LA_PARSE_SUCCESS;
                }
                else
                {
                    strcat(token_buffer, token_char);
                    retval = get_lexeme_string(source_file, token_buffer, blen, LEXEME_VAR_STRINGS, LEXEME_VAR_ARRAY_LEN);
                    
                    if (retval != LA_PARSE_SUCCESS)
                    {
                        fprintf(stderr, "Error: Lexical token error: Invalid token %s\n", token_buffer);
                    }
                }
                    
                break;
            default:
                if (strlen(token_buffer) >= blen - 1)
                {
                    fprintf(stderr, "Error: Token %s to large\n", token_buffer);
                    retval = LA_PARSE_ERROR;
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

int LA_get_token(lexical_analyzer_t *lexer, char **current_token)
{
    if (!lexer->source_file_stream || !lexer->source_file_location || !lexer->current_token)
    {
        return LA_PARSE_ERROR; 
    }
    
    int retval;
    FILE *source_file = lexer->source_file_stream;
    char *token_buffer = lexer->current_token;
    *current_token = token_buffer;

    memset(token_buffer, '\0', LA_MAX_TOKEN_SIZE);

    retval = get_token(source_file, token_buffer, LA_MAX_TOKEN_SIZE);

    if (retval != LA_PARSE_SUCCESS)
    {
        memset(token_buffer, '\0', LA_MAX_TOKEN_SIZE);
    }

    return retval;
}

lexical_analyzer_t *LA_create_new(lexical_analyzer_t **lexer, char *source_file_loc)
{
    if (!lexer || !source_file_loc || strlen(source_file_loc) < 1)
    {
        return NULL;
    }
    
    size_t file_loc_len = strlen(source_file_loc); 
    lexical_analyzer_t *retval = NULL;
    *lexer = (lexical_analyzer_t *) malloc(sizeof(**lexer));
    retval = *lexer;

    if (retval)
    {
        retval->source_file_stream = NULL;
        retval->source_file_location = NULL;
        retval->current_token = NULL;
        retval->token_len = 0;
        
        retval->source_file_stream = fopen(source_file_loc, "r"); 
        if (!retval->source_file_stream) 
        {
            fprintf(stderr, "Error: Unable to open file %s\n", source_file_loc);
            goto fopen_failed;
        }

        retval->source_file_location = (char *) malloc(file_loc_len + 1);
        if (!retval->source_file_location)
        {
            fprintf(stderr, "Error: Unable to allocate memory for the str %s\n", source_file_loc);
            goto malloc_file_loc_fail;
        }
        
        memset(retval->source_file_location, '\0', file_loc_len + 1);
        strncpy(retval->source_file_location, source_file_loc, file_loc_len); 
        
        retval->current_token = (char *) malloc(LA_MAX_TOKEN_SIZE);
        if (!retval->current_token)
        {
            fprintf(stderr, "Error: Unable to allocate memory for the str %s\n", source_file_loc);
            goto malloc_token_str_fail;
        }

        memset(retval->current_token, '\0', LA_MAX_TOKEN_SIZE);
    }
    else
    {
        fprintf(stderr, "Error: Malloc failed in LA_create_new\n");
    }

    return retval;

// Some error occured free memory accordingly 
malloc_token_str_fail:
    free(retval->source_file_location);
malloc_file_loc_fail:
    fclose(retval->source_file_stream);
fopen_failed:
    free(retval);
    retval = NULL;
    return retval;
}

void LA_free(lexical_analyzer_t **lexer_analyzer)
{
    lexical_analyzer_t *lexer = *lexer_analyzer;

    if (lexer)
    {
        lexer = *lexer_analyzer;

        if (lexer->source_file_location)
        {
            free(lexer->source_file_location);
            lexer->source_file_location = NULL;
        }
        
        if (lexer->current_token)
        {
            free(lexer->current_token);
            lexer->current_token = NULL;
        }

        if (lexer->source_file_stream)
        {
            fclose(lexer->source_file_stream);
            lexer->source_file_stream = NULL;
        }

        free(lexer);
        lexer = NULL;
        *lexer_analyzer = lexer;
    }
}






















