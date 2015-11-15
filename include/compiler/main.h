#ifndef MAIN_H
#define MAIN_H

#include <string.h>
#include <ctype.h>

#include "../ds/common.h"
#include "../ds/queue.h"

#define SOURCE_FILE_EXTENSION ".mkd"

typedef struct
{
    int val;
    const char *str;
} value_str_t;

// Returns 1 if the string ends with the suffix.
extern int str_ends_with(char *, char *);
// Prints the q
extern void print_queue(queue *q);
// Converts a string to lower case
// This function destroys the original values of str.
extern void str_tolower(char *str, size_t len);

#endif
