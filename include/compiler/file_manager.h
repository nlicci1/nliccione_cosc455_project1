// file_manager.h/.c is a simple wrapper used to mantain information about a file,
// hold a stream reference to an opened file,
// maintain a buffer that is dynamically allocted that is used to read from or write to the file.

// Maximum size of the current_token buffer
#define LA_MAX_TOKEN_SIZE 1000

#include <stdio.h>

typedef struct buffered_stream_manager
{
    // Reference to the file stream
    FILE *fstream;
    // Name of the file fstream is reading or writing 
    char *file_name;
    // Buffer is used to hold a str that will be written to fstream
    // or read from and written to fstream
    char *buffer;
    // Current buffer length
    size_t len;
} file_manager_t;

// This function creates a new lexical analyzer structure.
// Parameters:
//  The first argument is a pointer to a pointer used to point to the new LA
//  The second argument is the file location of the source file
// Returns:
//  NULL on failure or pointer to the newly allocated LA.
extern file_manager_t *FM_create_new(file_manager_t **, const char *, const char *);
// Frees all resources used by the LA structure and sets all the pointers in the struct to NULL
extern void FM_free(file_manager_t **);

