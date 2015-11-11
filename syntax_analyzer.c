#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "main.h"
#include "syntax_analyzer.h"
#include "lexemes.h"

static int is_whitespace(char *str)
{
    if (!str)
    {   
        return FALSE;
    }

    size_t len = strlen(str);
    int i;

    for (i = 0; i < len; i++)
    {
        if (!isspace(str[i]))
        {
            return FALSE;
        }
    }

    return TRUE;
}

// Returns TRUE if only plain text is in the first element of the str
// FALSE Is returned if the first char in str is a special syntax character
static int istext(char *str)
{
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
            return FALSE;
        }
    }
    
    return TRUE;
}

static char *get_token(syntax_analyzer_t *syn)
{
    if (!syn || !syn->lexer)
    {
        return NULL;
    }
    else if (syn->current_token)
    {
        return syn->current_token; 
    }
    
    lexical_analyzer_t *lexer = syn->lexer;
    char **retval = &syn->current_token; 
    char *tmp_location = NULL;
    size_t token_len;
    
    // Have the lexer get the next token
    if (LA_get_token(lexer, &tmp_location) == LA_PARSE_SUCCESS)
    {   
        // Allocate memory for the new token and copy the token into that new location
        token_len = strlen(tmp_location);
        if (token_len && (*retval = (char *) malloc(token_len + 1)))
        {
            memset(*retval, '\0', token_len + 1);
            strncpy(*retval, tmp_location, token_len);
        }
    } 
    
    return *retval;
}

// Adds current_token element into the parse tree (the queue)
// This function sets the pointer to the element to null so it can be reused
// when get_token is called again
static void update_parse_tree(syntax_analyzer_t *sya)
{
    if (sya)
    {
        if (sya->parse_tree)
        {
            queue_enqueue(sya->parse_tree, &sya->current_token);
            sya->current_token = NULL;
        }
    }
}

// This function implements a small subset of the simpler grammar.
// This functon implements a syntax check for a terminal symbol followed by TEXT 
// and then the ending terminal symbol. 
// Please note: This should only be used for productions that are optional
// For example: 
// <title> ::= TITLEB TEXT TITLEE | ε 
// or it could be a production check like this 
// <bold> ::= BOLD TEXT BOLD | ε
// Parameters:
//  syntax_analyzer_t * - Pointer to a syntax obj
//  lexeme_chars_t - Starting terminal symbol
//  lexeme_chars_t - Ending terminal symbol
static int syntax_opt_production(syntax_analyzer_t *syn, lexeme_chars_t terminal_start, lexeme_chars_t terminal_end)
{
    if (!syn)
    {
       return SYN_ANALYZER_PARSE_ERROR;
    }
    
    char *token = NULL;
    int retval = SYN_ANALYZER_PARSE_ERROR;
    
    if ((token = get_token(syn)))
    {
	    if (token[0] == terminal_start)
        {
            update_parse_tree(syn); 
	        if ((token = get_token(syn)) && istext(token))
	        {
                update_parse_tree(syn);
		        if ((token = get_token(syn)))
		        {
		            if (token[0] == terminal_end)
		            {
		                update_parse_tree(syn);
		                retval = SYN_ANALYZER_PARSE_SUCCESS;
		            }
		        }
	        }
	    }
        else
        {
            // We set retval to success because the symbol that was found did not belong to
	        // this optional production.
	        retval = SYN_ANALYZER_PARSE_SUCCESS;
        }
    }

    return retval;
} 

static int head(syntax_analyzer_t *sya)
{
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }

    int retcode = SYN_ANALYZER_PARSE_SUCCESS;
    char *tmp = get_token(sya);
    
    // If we have a token lets check it against the head sytnax
    // else it is an error if we do not get a token returned
    if (tmp)
    {
        if (tmp[0] == HEAD)
        {
            update_parse_tree(sya);
            retcode = syntax_opt_production(sya, TITLEB, TITLEE); 

            if (retcode != SYN_ANALYZER_PARSE_ERROR)
            {
                tmp = get_token(sya);

                if (tmp[0] != HEAD)
                {
                    retcode = SYN_ANALYZER_PARSE_ERROR; 
                }
                else
                {
                    update_parse_tree(sya);
                }
            }
        }
    }
    else
    {
        retcode = SYN_ANALYZER_PARSE_ERROR;
    }

    return retcode;
}

void SYN_print_parse_tree(syntax_analyzer_t *sya)
{
    if (sya)
    {
        char *tmp = malloc(sizeof (char *));
    
        if (sya->parse_tree)
        {
            while (sya->parse_tree->list->head != NULL)
            {
                queue_dequeue(sya->parse_tree, tmp);
                printf("|%s|\n", *(char **) tmp);
                free(* (char **)tmp);
            }
        }

        free(tmp);
    }
}

static void SYN_free_parse_tree(syntax_analyzer_t *sya)
{
    if (sya)
    {
        if (sya->parse_tree)
        {
            queue_destroy(sya->parse_tree);
            free(sya->parse_tree);
            sya->parse_tree = NULL;
        }
    }
}

int SYN_check_syntax(syntax_analyzer_t *sya)
{
    char *tmp = NULL;
    int retval = SYN_ANALYZER_PARSE_ERROR;
    // <head> ::= HEAD <title> HEAD | ε
    // <title> ::= TITLEB TEXT TITLEE | ε
    // <markdown> ::= DOCB <variable-define> <head> <body> DOCE
   
    tmp = get_token(sya);
    if (tmp)
    {
        if (strncasecmp(tmp, LEXEME_DOC_STRINGS[LEXEME_DOCB_IDX].str, strlen(tmp)) == 0)
        {
            update_parse_tree(sya);
            retval = head(sya);
            
            // Check for DOCE
            if (retval == SYN_ANALYZER_PARSE_SUCCESS)
            {
                if ((tmp = get_token(sya)) && (strncasecmp(tmp, LEXEME_DOC_STRINGS[LEXEME_DOCE_IDX].str, strlen(tmp)) == 0))
                {
                    update_parse_tree(sya);
                }
                else
                {
                    retval = SYN_ANALYZER_PARSE_ERROR;
                    fprintf(stderr, "Error: Syntax error. Expected: %s Found: %s\n", LEXEME_DOC_STRINGS[LEXEME_DOCE_IDX].str, tmp);
                }
            }
            else
            {
                fprintf(stderr, "Error: Syntax error invalid <head> | <title>.\nCurrent unexpected token found is: %s\n", tmp = get_token(sya));
            }
        }
        else
        {
            fprintf(stderr, "Error: Syntax error. Expected: %s Found: %s\n", LEXEME_DOC_STRINGS[LEXEME_DOCB_IDX].str, tmp);
        }
    }
    else 
    {
        fprintf(stderr, "Error: compile failed no token to check\n");
    }
    
    if (retval != SYN_ANALYZER_PARSE_SUCCESS)
    {
        SYN_free_parse_tree(sya);
    }

    return retval;
}

static void free_string(void *ptr)
{
    if (ptr)
    {
        free(*(char **) ptr);
    }
}

syntax_analyzer_t *SYN_create_new(syntax_analyzer_t **syn, lexical_analyzer_t *lex)
{
    if (!syn || !lex)
    {
        return NULL;
    }

    syntax_analyzer_t *retval = NULL;
    *syn = (syntax_analyzer_t *) malloc(sizeof(**syn));
    retval = *syn; 

    if (retval)
    {
        retval->lexer = lex;
        retval->current_token = NULL;
        
        if ((retval->parse_tree = malloc(sizeof(queue))))
        {
            queue_new(retval->parse_tree, sizeof(char *), free_string);
        }
    }
    
    return retval;
}


void SYN_free(syntax_analyzer_t **sya_analyzer)
{   
    syntax_analyzer_t *sya = *sya_analyzer;

    if (sya)
    {
        sya->lexer = NULL;
        
        SYN_free_parse_tree(sya);

        if (sya->current_token)
        {
            free(sya->current_token);
            sya->current_token = NULL;
        }

        free(sya);
        sya = NULL;
        *sya_analyzer = sya;
    }
}














