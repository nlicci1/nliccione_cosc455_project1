#include <stdlib.h>
#include "string.h"
#include "semantic_analyzer.h"
#include "lexemes.h"

/*
 * void sem_compile_single_char(sem_t *, char *compiled_lexeme);
 *
 */

struct lexeme_to_html_translation_entry
{
    // Value string entry that describes the markdown sybmol
    value_str_t lexeme_markdown_entry;
    // Handler function for the lexeme_markdown_entry
    // This function will now how to compile the entry
    compile_production_cb func;
    // This is the first argument to the callback function.
    // It will either be a string that contains the starting 
    // compiled lexeme or NULL (if not used).
    const char *first_func_arg;
};

// This structre and array of the compile_symbol_handler_lookup holds the neccessary information and
// routines to compile the current lexeme token. 
#define SEM_LEXEME_STR_ARRAY_SIZE  1

struct lexeme_to_html_translation_entry lexeme_compile_lookup_table[] = 
{   
    /*
    { DOC_LEXEME, "#" },
    { VAR_LEXEME, "$" },
    { HEAD, "^" },
    { TITLEB, "<" },
    { TITLEE, ">" },
    { PARAB, "{" },
    { PARAE, "}" },
    { EQSIGN, "=" },
    { ITALICS, "*" },
    */

    { { LISTITEMB, "+" }, NULL, "<li>" }
    //{ LISTITEME, ";" } },

    /*
    { NEWLINE, "~" },
    { LINKB, "[" },
    { LINKE, "]" },
    { AUDIO, "@" },
    { VIDEO, "%" },
    { ADDRESSB, "(" },
    { ADDRESSE, ")" }
    */ 
};

static void init_lookup_table(hashtable_t **lookup_tb)
{
    int i;

    *lookup_tb = ht_create_new(32, sizeof(char *), NULL);
    
    for (i = 0; i < SEM_LEXEME_STR_ARRAY_SIZE; i++)
    {
        ht_insert(*lookup_tb, (char *) lexeme_compile_lookup_table[i].lexeme_markdown_entry.str, &lexeme_compile_lookup_table[i]);
    }
}

static void parse_tree_entry_element_free(void *ele)
{
    if (ele)
    {
        free(* (char **) ele);
    }
}

void SEM_create_new(sem_t *sem, queue *parse_tree, char *html_file_name)
{
    hashtable_t *lexeme_lookup_tb = NULL;
    hashtable_t *var_resolution_table = NULL;

    if (sem && html_file_name)
    {

        sem->markdown_parse_tree = parse_tree;
        sem->compiled_parse_tree = NULL;
        sem->file_operator = NULL;
        sem->symbol_key_list = NULL;
        sem->symbol_table = NULL;
        sem->variable_scope_level = 0;
        init_lookup_table((hashtable_t **) &sem->lexeme_lookup_table); 
        
        
        sem->compiled_parse_tree = malloc(sizeof(sem->compiled_parse_tree));
        memset(sem->compiled_parse_tree, 0, sizeof(sem->compiled_parse_tree));
        remove(html_file_name);
        FM_create_new(&sem->file_operator, html_file_name, "w");
        queue_new(sem->compiled_parse_tree, sizeof(char *), parse_tree_entry_element_free);
    }
}

void SEM_free(sem_t *sem)
{
    if (sem)
    {
        if (sem->file_operator)
        {
            FM_free(&sem->file_operator);
            sem->file_operator = NULL;
        }
    
        if (sem->compiled_parse_tree)
        {
           queue_destroy(sem->compiled_parse_tree);
           free(sem->compiled_parse_tree);
           sem->compiled_parse_tree = NULL;
        }
        
        if (sem->lexeme_lookup_table)
        {
            ht_free((hashtable_t *) sem->lexeme_lookup_table);
        }
    }
}

















