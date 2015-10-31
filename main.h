#ifndef MAIN_H
#define MAIN_H

#define MAX_TOKEN_SIZE 1000
#define SOURCE_FILE_EXTENSION ".mkd"
#define TRUE 1
#define FALSE 0

typedef struct
{
    int val;
    char *str;
} value_str_t;

// Returns 1 if the string ends with the suffix.
extern int str_ends_with(char *, char *);

#endif
