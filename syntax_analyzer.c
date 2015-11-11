#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>

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
            
            // If token is only white space
            // Add it straight to the parse tree
            // And get next token
            if (is_whitespace(*retval))
            {
                update_parse_tree(syn);
                *retval = get_token(syn);
            }
        }
    } 
    
    return *retval;
}


// This function checks to see if the current token matches the terminal character symbol
// Returns:
// SYN_ANALYZER_PARSE_SUCCESS - If the terminal matches the current token input and was added to the parse tree
// SYN_ANALYZER_PARSE_ERROR - If a syntax error occured
// SYN_ANALYZER_EMPTY_TOKEN - If get_token returns a NULL pointer
static int syntax_check_char(syntax_analyzer_t *syn, lexeme_chars_t terminal)
{
    if (!syn)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }

    char *token = NULL;
    int retval = SYN_ANALYZER_PARSE_ERROR;

    if ((token = get_token(syn)))
    {
        if (strlen(token) == 1 && token[0] == terminal)
        {
            update_parse_tree(syn); 
            retval = SYN_ANALYZER_PARSE_SUCCESS;
        }
    }
    else
    {
        retval = SYN_ANALYZER_EMPTY_TOKEN;
    }

    return retval;
}

// Checks to see if the current token matches the terminal lexeme string
// If it does then add it into the parse tree else
// return error
static int syntax_check_str(syntax_analyzer_t *syn, const char *terminal)
{
    if (!syn || !terminal)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }

    char *token = NULL;
    size_t token_len;
    size_t terminal_strlen;
    int retval = SYN_ANALYZER_PARSE_ERROR;

    if ((token = get_token(syn)))
    {
        token_len = strlen(token);
        terminal_strlen = strlen(terminal);
        if ((token_len == terminal_strlen) && (strncasecmp(token, terminal, token_len) == 0))
        {
            update_parse_tree(syn); 
            retval = SYN_ANALYZER_PARSE_SUCCESS;
	    }
    }
    else
    {
        retval = SYN_ANALYZER_EMPTY_TOKEN;
    }

    return retval;
}

// Checks to see if the current token is plain text
// If it is then add it into the parse tree else
// return error.
static int syntax_check_plain_text(syntax_analyzer_t *syn)
{
    if (!syn)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }

    char *token = NULL;
    int retval = SYN_ANALYZER_PARSE_ERROR;

    if ((token = get_token(syn)))
    {
        if (istext(token))
        {
            update_parse_tree(syn); 
            retval = SYN_ANALYZER_PARSE_SUCCESS;
        }
    }
    else
    {
        retval = SYN_ANALYZER_EMPTY_TOKEN;
    }

    return retval;

}

// This functon is a generic syntax checker for a production. It will add syntactically correct symbols to the
// parse tree if they follow the production rules. 
// For example: 
// <title> ::= TITLEB TEXT TITLEE | ε 
// or it could be a production check like this 
// <bold> ::= BOLD TEXT BOLD | ε
// Parameters:
//  syntax_analyzer_t * - Pointer to a syntax obj
//  const * - The format list where each character represents the type of the variable passed. 
//      The valid types are:
//          c - The current variable is a char and will be matched with current token
//          s - The current variable is a string and will be matched with current token
//          T - The current variable is NULL and allows any plain text in the current token
//  ... - Is the variable number of arguments. Based on the const *format list each variable is checked against
//  the current input token. 
// Returns:
//  SYN_ANALYZER_EMPTY_TOKEN - If get_token returns a NULL pointer
//  SYN_ANALYZER_PARSE_ERROR - If the syntax rules were violated
//  SYN_ANALYZER_PARSE_SUCCESS - If all symbols that make up this production were added into the parse tree
static int syntax_check_production(syntax_analyzer_t *syn, char *format, ...)
{
    if (!syn || !format || *format == '\0')
    {
       return SYN_ANALYZER_PARSE_ERROR;
    }
    
    va_list syntax_rule;
    int retval = SYN_ANALYZER_PARSE_ERROR;
    
    va_start(syntax_rule, format);

    while (*format != '\0')
    {
        switch (*format)
        {
            case 'c':
                retval = syntax_check_char(syn, va_arg(syntax_rule, int));
                break;
            case 's':
                retval = syntax_check_str(syn, va_arg(syntax_rule, char *));
                break;
            case 'T':
                va_arg(syntax_rule, void *);
                retval = syntax_check_plain_text(syn);
                break;
            default:
                fprintf(stderr, "Error: Invalid format. Expected (c | s | T)\n");
                retval = SYN_ANALYZER_PARSE_ERROR;
                break;
        }

        if (retval != SYN_ANALYZER_PARSE_SUCCESS)
        {
            break;
        }

        format++;
    }
    
    return retval;
}

// This function implements the optional production variable define as:
// <variable-define> ::= DEFB TEXT EQSIGN TEXT DEFUSEE <variable-define> | ε
static int var_define(syntax_analyzer_t *sya)
{
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }
    
    const char *var_start_def = LEXEME_VAR_STRINGS[LEXEME_DEFB_IDX].str;
    const char *var_end_def = LEXEME_VAR_STRINGS[LEXEME_DEFUSEE_IDX].str;
    int retcode;
    lexeme_chars_t var_set_value_token = EQSIGN;
    
    // Check for our first terminal symbol
    // If its there than we keep trucking on thru
    // the BNF train.
    // If is not present this an optional symbol return
    // opt success 
    retcode = syntax_check_str(sya, var_start_def); 
    
    if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
    {
        retcode = syntax_check_production(sya, "TcTs", NULL, var_set_value_token, NULL, var_end_def); 

        if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
        {
            retcode = var_define(sya);
        }
    }
    else if (retcode == SYN_ANALYZER_PARSE_ERROR)
    {
        retcode = SYN_ANALYZER_PARSE_OPT_SUCCESS;
    }

    return retcode;
}

// This function implements the optional production variable define as:
// <variable-use> ::= USEB TEXT DEFUSEE | ε
static int var_use(syntax_analyzer_t *sya)
{
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }
    
    const char *var_start_use = LEXEME_VAR_STRINGS[LEXEME_USEB_IDX].str;
    const char *var_end_use = LEXEME_VAR_STRINGS[LEXEME_DEFUSEE_IDX].str;
    int retcode;
    
    // Check for our first terminal symbol
    // If its there than we keep trucking on thru
    // the BNF train.
    // If is not present this an optional symbol return
    // success 
    retcode = syntax_check_str(sya, var_start_use); 
    
    if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
    {
        retcode = syntax_check_production(sya, "Ts", NULL, var_end_use);
    }
    else if (retcode == SYN_ANALYZER_PARSE_ERROR)
    {
        retcode = SYN_ANALYZER_PARSE_OPT_SUCCESS;
    }

    return retcode;
}

// This function implements the optional production variable define as:
// <bold> ::= BOLD TEXT BOLD | ε
static int bold(syntax_analyzer_t *sya)
{
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }
    
    const char *bold_terminal = "**";
    int retcode;
    
    // Check for our first terminal symbol
    // If its there than we keep trucking on thru
    // the BNF train.
    // If is not present this an optional symbol return
    // success 
    retcode = syntax_check_str(sya, bold_terminal); 
    
    if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
    {
        retcode = syntax_check_production(sya, "Ts", NULL, bold_terminal);
    }
    else if (retcode == SYN_ANALYZER_PARSE_ERROR)
    {
        retcode = SYN_ANALYZER_PARSE_OPT_SUCCESS;
    }

    return retcode;
}

// This function implements the optional production variable define as:
// <italics> ::= ITALICS TEXT ITALICS | ε
static int italics(syntax_analyzer_t *sya)
{
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }

    lexeme_chars_t italics_terminal = ITALICS;
    int retcode;
    
    // Check for our first terminal symbol
    // If its there than we keep trucking on thru
    // the BNF train.
    // If is not present this an optional symbol return
    // success 
    retcode = syntax_check_char(sya, italics_terminal); 
    
    if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
    {
        retcode = syntax_check_production(sya, "Ts", NULL, italics_terminal);
    }
    else if (retcode == SYN_ANALYZER_PARSE_ERROR)
    {
        retcode = SYN_ANALYZER_PARSE_OPT_SUCCESS;
    }

    return retcode;
}

// This function implements the production rule head defined as:
// <url> ::= ADDRESSB TEXT ADDRESSE
static int url(syntax_analyzer_t *sya)
{
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }
    
    lexeme_chars_t addr_start = ADDRESSB;
    lexeme_chars_t addr_end = ADDRESSE;
    
    return syntax_check_production(sya, "cTc", addr_start, NULL, addr_end);
}

// <link> ::= LINKB TEXT LINKE ADDRESSB TEXT ADDRESSE | ε
static int link(syntax_analyzer_t *sya)
{   
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }
    
    lexeme_chars_t link_start = LINKB;
    lexeme_chars_t link_end = LINKE;
    int retcode;

    // Check for our first terminal symbol
    // If its there than we keep trucking on thru
    // the BNF train.
    // If is not present this an optional symbol return
    // success 
    retcode = syntax_check_char(sya, link_start); 
    
    if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
    {
        retcode = syntax_check_production(sya, "Tc", NULL, link_end); 

        if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
        {
            retcode = url(sya);          
        }
    }
    else if (retcode == SYN_ANALYZER_PARSE_ERROR)
    {
        retcode = SYN_ANALYZER_PARSE_OPT_SUCCESS;
    }
    
    return retcode;

}

// <audio> ::= AUDIO ADDRESSB TEXT ADDRESSE | ε
static int audio(syntax_analyzer_t *sya)
{   
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }
    
    lexeme_chars_t audio_start = AUDIO;
    int retcode;

    // Check for our first terminal symbol
    // If its there than we keep trucking on thru
    // the BNF train.
    // If is not present this an optional symbol return
    // success 
    retcode = syntax_check_char(sya, audio_start); 
    
    if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
    {
        retcode = url(sya);          
    }
    else if (retcode == SYN_ANALYZER_PARSE_ERROR)
    {
        retcode = SYN_ANALYZER_PARSE_OPT_SUCCESS;
    }
    
    return retcode;
}

// <video> ::= VIDEO ADDRESSB TEXT ADDRESSE | ε
static int video(syntax_analyzer_t *sya)
{   
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }
    
    lexeme_chars_t video_start = VIDEO;
    int retcode;

    // Check for our first terminal symbol
    // If its there than we keep trucking on thru
    // the BNF train.
    // If is not present this an optional symbol return
    // success 
    retcode = syntax_check_char(sya, video_start); 
    
    if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
    {
        retcode = url(sya);          
    }
    else if (retcode == SYN_ANALYZER_PARSE_ERROR)
    {
        retcode = SYN_ANALYZER_PARSE_OPT_SUCCESS;
    }
    
    return retcode;
}

// This function implements the production rule head defined as:
// TITLEB TEXT TITLEE | ε
static int title(syntax_analyzer_t *sya)
{
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }

    lexeme_chars_t title_start = TITLEB;
    lexeme_chars_t title_end = TITLEE;
    int retcode;
    
    // Check for our first terminal symbol
    // If its there than we keep trucking on thru
    // the BNF train.
    // If is not present this an optional symbol return
    // success 
    retcode = syntax_check_production(sya, "c", title_start); 

    if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
    {
        retcode = syntax_check_production(sya, "Tc", NULL, title_end); 
    }
    else if (retcode == SYN_ANALYZER_PARSE_ERROR)
    {
        retcode = SYN_ANALYZER_PARSE_OPT_SUCCESS;
    }
    
    return retcode;
}

// This function implements the production rule head defined as:
// <head> ::= HEAD <title> HEAD | ε
static int head(syntax_analyzer_t *sya)
{   
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }

    lexeme_chars_t title_start = HEAD;
    lexeme_chars_t title_end = HEAD;
    int retcode;
    
    // Check for our first terminal symbol
    // If its there than we keep trucking on thru
    // the BNF train.
    // If is not present this an optional symbol return
    // success 
    retcode = syntax_check_production(sya, "c", title_start); 

    if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
    {
        retcode = title(sya);

        if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
        {
            retcode = syntax_check_production(sya, "c", title_end); 
        }
    }
    else if (retcode == SYN_ANALYZER_PARSE_ERROR)
    {
        retcode = SYN_ANALYZER_PARSE_OPT_SUCCESS;
    }
    
    return retcode;
}

// <inner-item> ::= <variable-use> <inner- item>
// | <bold> <inner- item>
// | <italics> <inner- item>
// | <link> <inner- item>
// | TEXT <inner- item>
// | ε
static int inner_item(syntax_analyzer_t *sya)
{
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }
    
    int retcode = SYN_ANALYZER_PARSE_ERROR;
    
    if (
        ((retcode = var_use(sya)) == SYN_ANALYZER_PARSE_SUCCESS) 
        || (retcode != SYN_ANALYZER_PARSE_ERROR && (retcode = bold(sya)) == SYN_ANALYZER_PARSE_SUCCESS) 
        || (retcode != SYN_ANALYZER_PARSE_ERROR && (retcode = italics(sya)) == SYN_ANALYZER_PARSE_SUCCESS)
        || (retcode != SYN_ANALYZER_PARSE_ERROR && (retcode = link(sya)) == SYN_ANALYZER_PARSE_SUCCESS) 
        || (retcode != SYN_ANALYZER_PARSE_ERROR && (retcode = syntax_check_plain_text(sya)) == SYN_ANALYZER_PARSE_SUCCESS)
       )
    {
        retcode = inner_item(sya);
    }

    return retcode;

}

// <listitem> ::= LISTITEMB <inner-item> LISTITEME <list-item> | ε 
static int list_item(syntax_analyzer_t *sya)
{
    if (!sya)
    {
        return SYN_ANALYZER_PARSE_ERROR;
    }
    
    lexeme_chars_t list_item_start = LISTITEMB;
    lexeme_chars_t list_item_end = LISTITEME;
    int retcode;
    
    // Check for our first terminal symbol
    // If its there than we keep trucking on thru
    // the BNF train.
    // If is not present this an optional symbol return
    // success 
    retcode = syntax_check_char(sya, list_item_start); 
    
    if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
    {
        retcode = inner_item(sya);
        if (retcode == SYN_ANALYZER_PARSE_SUCCESS || retcode == SYN_ANALYZER_PARSE_OPT_SUCCESS)
        {
            retcode = syntax_check_char(sya, list_item_end); 

            if (retcode == SYN_ANALYZER_PARSE_SUCCESS)
            {
                retcode = list_item(sya);
            }
        }
    }
    else if (retcode == SYN_ANALYZER_PARSE_ERROR)
    {
        retcode = SYN_ANALYZER_PARSE_SUCCESS;
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
    const char *doc_start = LEXEME_DOC_STRINGS[LEXEME_DOCB_IDX].str;
    const char *doc_end = LEXEME_DOC_STRINGS[LEXEME_DOCE_IDX].str;
    char *current_token = NULL;
    int retval;
    
    retval = syntax_check_str(sya, doc_start);

    if (retval == SYN_ANALYZER_PARSE_SUCCESS)
    {
        // Check syntax of any global variables
        retval = var_define(sya);
        
        if (retval == SYN_ANALYZER_PARSE_SUCCESS)
        {
            // Check for <head> and or <body> productions
            retval = head(sya);
        }
       
        // Move onto <body> DOCE
        if (retval == SYN_ANALYZER_PARSE_SUCCESS)
        {
            // Check for DOCE
            retval = syntax_check_str(sya, doc_end);
            if (retval != SYN_ANALYZER_PARSE_SUCCESS)
            {
                current_token = get_token(sya);
                fprintf(stderr, "Error: Syntax error. Expected: %s Found: %s\n", doc_end, current_token);
            }
        }
        else
        {
            current_token = get_token(sya);
            fprintf(stderr, "Error: Syntax error invalid <var-define> | <head> | <title>.\nCurrent unexpected token found is: %s\n", current_token);
        }
    }
    else
    {
        current_token = get_token(sya);
        fprintf(stderr, "Error: Syntax error. Expected: %s Found: %s\n", doc_start, current_token);
    }
    
    if (retval != SYN_ANALYZER_PARSE_SUCCESS)
    {
        // We do not want a broken parse tree being accessed outside of this obj file. So we destroy the parse tree.
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














