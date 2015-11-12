#ifndef SYNTAX_ANALYZER_H
#define SYNTAX_ANALYZER_H

// Returned if the syntax check if a production was successfull
// and everything was added to the parse tree
#define SYN_ANALYZER_PARSE_SUCCESS 3 
// Returned if an optional production's starting symbol
// was not found. This is a success return code only
// becuase the optional start symbol was not found
#define SYN_ANALYZER_PARSE_OPT_SUCCESS 4 
// Returned if a syntax violation occured.
// Meaning there was a syntax error in one of the grammar
// rules.
#define SYN_ANALYZER_PARSE_ERROR 5 
#define SYN_ANALYZER_EMPTY_TOKEN 6 

#include "lexical_analyzer.h"
#include "../ds/queue.h"

typedef struct syntax_analyzer
{
    lexical_analyzer_t *lexer;
    queue *parse_tree;
    // We will want to reuse the same pointer address so we can re-check the syntax if the current production 
    // is not required. Also, becuase we malloc memory for the token so we can add it to the parse tree 
    // and have it freed later in the program from the parse tree struct.
    char *current_token;
} syntax_analyzer_t;


void SYN_print_parse_tree(syntax_analyzer_t *sya);
int SYN_check_syntax(syntax_analyzer_t *);
syntax_analyzer_t *SYN_create_new(syntax_analyzer_t **, lexical_analyzer_t *);
void SYN_free(syntax_analyzer_t **);

#endif
