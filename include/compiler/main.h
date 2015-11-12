#ifndef MAIN_H
#define MAIN_H

#define SOURCE_FILE_EXTENSION ".mkd"
#include "../ds/common.h"

typedef struct
{
    int val;
    const char *str;
} value_str_t;

// Returns 1 if the string ends with the suffix.
extern int str_ends_with(char *, char *);

#endif
