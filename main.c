#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "lexical_analyzer.h"

int main(int argc, char **argv)
{
    char current_token[MAX_TOKEN_SIZE];
    FILE *source_file = NULL;
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


    source_file = fopen(source_file_loc, "r"); 
    if (source_file == NULL) 
    {
        fprintf(stderr, "Error: Unable to open file %s\n", source_file_loc);
        return EXIT_FAILURE;
    }

    while (TRUE)
    {
        retval = get_token(source_file, current_token, MAX_TOKEN_SIZE);

        if (strlen(current_token) > 0)
        {
            printf("|%s|\n", current_token);
        }

        if (retval != LEXICAL_PARSE_SUCCESS)
        {
            break;
        }
    }

    fclose(source_file);

    return EXIT_SUCCESS;
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
