#ifndef SEM_ANALYZER_H
#define SEM_ANALYZER_H

#include "file_manager.h"
#include "hash.h"
#include "list.h"
#include "queue.h"

// Return codes 
#define SEM_SUCCESS 7
#define SEM_FAILURE 8
#define SEM_VARIABLE_UNDEFINED_ERROR 9

// This is used to maintain a reference to a structure that holds the:
// open compiled file,
// holds the file path of the open file,
// contain a pointer to the current token that is being processed by the semantic analyzer, 
// and the size of the buffer allocated for current token
typedef file_manager_t semantic_token_processor_t;

typedef struct
{
    queue *markdown_parse_tree;
    queue *compiled_parse_tree;
    // Used to maintain a refernce to an open file and
    // the current string buffer that will be written to the file
    semantic_token_processor_t *file_operator; 
    // List of defined variables 
    list *symbol_key_list;
    // Mantains a table of defined values. The defined value (variable name)
    // maps to a stack of values that variable has and the scope level it was
    // defined in. Pro scope level idea by @kboyd34.
    hashtable_t *symbol_table;
    // This is automatically set and the user should never
    // have to change what this is pointing to and the data inside it
    const hashtable_t *const lexeme_lookup_table;
    // Level of scope for a variable defined with $def
    // This is incremented for each level of scope
    // scope_level = 0 this var has global scope
    // scope_level = 1 the variable has local scope
    // inside a paragraph
    unsigned int variable_scope_level;
} sem_t;

// Definition for a callback function that will handle
// a grammar rule. this means it will translate the token
// into HTML.
typedef int (* compile_production_cb) (sem_t *, const char *, char *);

// This function will check the semantics of the parse tree by resolving variables
// and writing the contents from the compiled parse tree to the document
// Returns:
//  SEM_SUCCESS - An html file was created
//  SEM_FAILURE - General failure
//  SEM_VARIABLE_UNDEFINED_ERROR - A variable was used but never defined
extern int SEM_compile(sem_t *);
extern void SEM_create_new(sem_t *sem, queue *parse_tree, char *html_file_name);
extern void SEM_free(sem_t *);

#endif
