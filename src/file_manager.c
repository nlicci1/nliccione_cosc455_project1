#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "file_manager.h"

file_manager_t *FM_create_new(file_manager_t **fmngr, const char *file_name, const char *fopen_mode)
{
    if (!fmngr || !file_name || strlen(file_name) < 1)
    {
        return NULL;
    }
    
    size_t file_loc_len = strlen(file_name); 
    file_manager_t *retval = NULL;
    *fmngr = (file_manager_t *) malloc(sizeof(**fmngr));
    retval = *fmngr;

    if (retval)
    {
        retval->fstream = NULL;
        retval->file_name = NULL;
        retval->buffer = NULL;
        retval->len = 0;
        
        retval->fstream = fopen(file_name, fopen_mode); 
        if (!retval->fstream) 
        {
            fprintf(stderr, "Error: Unable to open file %s\n", file_name);
            goto fopen_failed;
        }

        retval->file_name = (char *) malloc(file_loc_len + 1);
        if (!retval->file_name)
        {
            fprintf(stderr, "Error: Unable to allocate memory for the str %s\n", file_name);
            goto malloc_file_loc_fail;
        }
        
        memset(retval->file_name, '\0', file_loc_len + 1);
        strncpy(retval->file_name, file_name, file_loc_len); 
        
        retval->buffer = (char *) malloc(LA_MAX_TOKEN_SIZE);
        if (!retval->buffer)
        {
            fprintf(stderr, "Error: Unable to allocate memory for the str %s\n", file_name);
            goto malloc_buffer_str_fail;
        }

        memset(retval->buffer, '\0', LA_MAX_TOKEN_SIZE);
    }
    else
    {
        fprintf(stderr, "Error: Malloc failed in FM_create_new\n");
    }

    return retval;

// Some error occured free memory accordingly 
malloc_buffer_str_fail:
    free(retval->file_name);
malloc_file_loc_fail:
    fclose(retval->fstream);
fopen_failed:
    free(retval);
    retval = NULL;
    return retval;
}

void FM_free(file_manager_t **fmngr)
{
    file_manager_t *file_manager = *fmngr;

    if (file_manager)
    {
        if (file_manager->file_name)
        {
            free(file_manager->file_name);
            file_manager->file_name = NULL;
        }
        
        if (file_manager->buffer)
        {
            free(file_manager->buffer);
            file_manager->buffer= NULL;
        }

        if (file_manager->fstream)
        {
            fclose(file_manager->fstream);
            file_manager->fstream = NULL;
        }

        free(file_manager);
        file_manager = NULL;
        *fmngr = file_manager;
    }
}
