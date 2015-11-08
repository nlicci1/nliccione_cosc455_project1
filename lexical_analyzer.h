#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#define LA_MAX_TOKEN_SIZE 1000

#define LA_PARSE_SUCCESS 1
#define LA_PARSE_ERROR 2

typedef struct lexical_analyzer
{
        FILE *source_file_stream;
        char *source_file_location;
        char *current_token;
        size_t token_len;
} lexical_analyzer_t;

// This function creates a new lexical analyzer structure.
// Parameters:
//  The first argument is a pointer to a pointer used to point to the new LA
//  The second argument is the file location of the source file
// Returns:
//  NULL on failure or pointer to the newly allocated LA.
extern lexical_analyzer_t *LA_create_new(lexical_analyzer_t **, char *);
// Frees all resources used by the LA structure and sets all the pointers in the struct to NULL
extern void LA_free(lexical_analyzer_t **);
// This function gets the next token and points to it with the second parameter
// Parameters:
//  The first argument is a pointer to the lexical analyzer structure that is to be operated on
//  The second argument is a pointer to a pointer that will contain the current_token if the function
//  returns successfully
// Returns:
//  EOF if end of file has been reached
//  LA_PARSE_SUCCESS on successfull parse
//  LA_PARSE_ERROR if an error has occured
extern int LA_get_token(lexical_analyzer_t *, char **);

#endif
