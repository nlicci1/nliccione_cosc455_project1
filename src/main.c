#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"
#include "lexical_analyzer.h"
#include "syntax_analyzer.h"
#include "semantic_analyzer.h"

int main(int argc, char **argv)
{
    sem_t sem;
    lexical_analyzer_t *lexer = NULL;
    syntax_analyzer_t *sya = NULL;
    char *dst_file_loc = NULL;
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
    
    // Create output filename
    dst_file_loc = malloc(strlen(source_file_loc) + 2);
    memset(dst_file_loc, '\0', strlen(source_file_loc) + 2);
    strncpy(dst_file_loc, source_file_loc, strstr(source_file_loc, ".mkd") - source_file_loc);
    strcat(dst_file_loc, ".html");

    
    
    if (LA_create_new(&lexer, source_file_loc))
    {
        SYN_create_new(&sya, lexer);
        
        // kick off syntax checker function call thingy here
        retval = SYN_check_syntax(sya);
        
        // Normalize error codes
        if (retval == SYN_ANALYZER_PARSE_SUCCESS || retval == SYN_ANALYZER_PARSE_OPT_SUCCESS)
        {
            retval = EXIT_SUCCESS;
        }
        
        if (retval == EXIT_SUCCESS && sya->parse_tree)
        {
            SEM_create_new(&sem, sya->parse_tree, dst_file_loc);
            retval = SEM_compile(&sem);
            
            // Normalize return values to EXIT_SUCCESS if successfull
            if (retval == SEM_SUCCESS)
            {
                retval = EXIT_SUCCESS;
            }
            
            SEM_free(&sem);
        }

        SYN_free(&sya);
        LA_free(&lexer);
    }
    else
    {
        retval = EXIT_FAILURE;
    }

    free(dst_file_loc);
    
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

static bool print_list_item(void *item)
{
    if (item)
    {
        printf("|%s|\n", *(char **) item);
    }
    else
    {
        printf("|NULL|\n");
    }

    return TRUE;
}

void print_queue(queue *q)
{
    if (q->list)
    {
        list_for_each(q->list, print_list_item);
    }
}

void str_tolower(char *str, size_t len)
{
    int i;

    if (str && len > 0)
    {
        for (i = 0; i < len; i++)
        {
            str[i] = tolower(str[i]);
        }
    }
}



















