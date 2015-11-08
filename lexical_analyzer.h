#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#define LEXICAL_MAX_TOKEN_SIZE 1000

#define LEXICAL_PARSE_SUCCESS 1
#define LEXICAL_PARSE_ERROR 2

typedef struct lexical_analyzer
{
    FILE *source_file_stream;
    char *source_file_location;
    char *current_token;
    size_t token_len;
} lexical_analyzer_t;

// This function creates a new lexical analyzer structure.
// Parameters
//  The first argument is a pointer to a pointer used to point to the new LA
//  The second argument is the file location of the source file
// Returns:
//  NULL on failure or pointer to the newly allocated LA.
extern lexical_analyzer_t *LA_create_new(lexical_analyzer_t **, char *);
// Frees all resources in the LA structure and sets all the pointers in the struct to NULL
extern void LA_free(lexical_analyzer_t **);
// This function gets the next token from the source file.
extern int LA_get_token(lexical_analyzer_t *);

#endif
