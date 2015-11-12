#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "lexical_analyzer.h"
#include "syntax_analyzer.h"

int main(int argc, char **argv)
{
    lexical_analyzer_t *lexer;
    syntax_analyzer_t *sya;
    char *source_file_loc = NULL;
    int retval = EXIT_SUCCESS;

    if (argc != 2)
    {
        fprintf(stderr, "Error: Invalid number of command line arguments (program_name input_source_file.mkd)\n");
        return EXIT_FAILURE;
    }
    
    source_file_loc = argv[1];
    
    if (str_ends_with(source_file_loc, SOURCE_FILE_EXTENSION) == FALSE)
    {
        fprintf(stderr, "Error: Invalid file extension.\n");
        return EXIT_FAILURE;
    }

    LA_create_new(&lexer, source_file_loc);
    SYN_create_new(&sya, lexer);
    
    // kick off syntax checker function call thingy here
    SYN_check_syntax(sya);
    
    // If we have a parse tree 
    // Begin semantics
    if (sya->parse_tree)
    {
    }
    
    SYN_free(&sya);
    LA_free(&lexer);

    return retval;
}

int str_ends_with(char *str, char *suffix)
{
    char *suffix_end_ptr;
    char *substr_ptr;
    size_t str_len;
    size_t suffix_len;
    int retval = TRUE;

    if (str == NULL || suffix == NULL)
    {
        return FALSE;
    }
    
    str_len = strlen(str);
    suffix_len = strlen(suffix);
    
    if (str_len == 0 || suffix_len == 0 || suffix_len > str_len)
    {
        return FALSE;
    }

    suffix_end_ptr = suffix + strlen(suffix) - 1;
    substr_ptr = str + strlen(str) - 1;
    while (substr_ptr >= str && suffix_end_ptr >= suffix)
    {
        if (*substr_ptr != *suffix_end_ptr)
        {
            retval = FALSE;
            break;
        }
        
        suffix_end_ptr--;
        substr_ptr--; 
    }

    return retval;

}
