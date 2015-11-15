#include <stdlib.h>
#include <string.h>

#include "semantic_analyzer.h"
#include "lexemes.h"

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

// The following elements that use this callback func must have a 
// simple 1 to 1 relationship between the markdown lexeme and the html.
// This means we can simply 'swap out' the two strings without
// having to worry about any extra processing.
static int sem_compile_lexeme_ez(sem_t *semantic_analyzer, const char *compiled_lexeme, char *markdown_lexeme)
{
    char *tmp = NULL;

    if (compiled_lexeme)
    {
        tmp = strdup(compiled_lexeme);
        queue_enqueue(semantic_analyzer->compiled_parse_tree, &tmp);
        return SEM_SUCCESS;
    }
    else
    {
        fprintf(stderr, "Error: Compile failed for %s. Could not be translated to HTML\n", markdown_lexeme);
        return SEM_FAILURE;
    }
}

// This structre and array holds the neccessary information and
// routines to compile the current lexeme token. 
#define SEM_LEXEME_COMPILE_SYMBOL_ARRAY_SIZE  21
// This is table of callbacks used to handle (compile) each lexeme.
// Eache table element will be directly mapped to by the use of a hashtable lookup
struct lexeme_to_html_translation_entry lexeme_compile_lookup_table[] = 
{  
    // The following elements have a simple 1 to 1
    // relationship between the markdown lexeme and the html.
    // This means we can simply 'swap out' the two strings without
    // having to worry about any extra processing.
    { { DOC_LEXEME, "#begin" }, sem_compile_lexeme_ez, "<html>" },
    { { DOC_LEXEME, "#end" }, sem_compile_lexeme_ez, "</html>" },
    { { TITLEB, "<" }, sem_compile_lexeme_ez, "<title>" },
    { { TITLEE, ">" }, sem_compile_lexeme_ez, "</title>" },
    { { PARAB, "{" }, sem_compile_lexeme_ez, "<p>" },
    { { PARAE, "}" }, sem_compile_lexeme_ez, "</p>" },
    { { LISTITEMB, "+" }, sem_compile_lexeme_ez, "<li>" },
    { { LISTITEME, ";" }, sem_compile_lexeme_ez, "</li>" },
    { { NEWLINE, "~" }, sem_compile_lexeme_ez, "<br>" },
    { { LINKE, "]" }, sem_compile_lexeme_ez, "</a>" }, 
    // The %s will be used to easily place the URL inside the link
    { { LINKB, "[" }, NULL, "<a href=\"%s\">" },
    { { AUDIO, "@" }, NULL, "<audio controls> <source src=\"%s\"> </audio>" },
    { { VIDEO, "%" }, NULL, "<iframe src=\"%s\">" },
    { { VAR_LEXEME, "$def" }, NULL, NULL },
    { { VAR_LEXEME, "$use" }, NULL, NULL },
    { { HEAD, "^" }, NULL, "<head>" },
    { { ITALICS, "*" }, NULL,  "<i>" },
    { { BOLD, "**" }, NULL, "<b>" },
    // There is no special function for handling these symbols
    //{ { ADDRESSB, "(" }, NULL, NULL },
    //{ { ADDRESSE, ")" }, NULL, NULL }
    //{ { EQSIGN, "=" }, NULL, NULL },
};

int SEM_compile(sem_t *sema)
{
    if (!sema)
    {
        return SEM_FAILURE;
    }
    
    struct lexeme_to_html_translation_entry **entry = NULL;
    struct lexeme_to_html_translation_entry *current_conversion_entry = NULL;
    queue *parse_tree = NULL;
    hashtable_t *lexeme_lookup_tb = NULL;
    char *current_lexeme = NULL;
    unsigned int *var_scope_level = &sema->variable_scope_level;
    int retval = SEM_SUCCESS;
    
    lexeme_lookup_tb = (hashtable_t *) sema->lexeme_lookup_table;
    parse_tree = sema->markdown_parse_tree;
   
    // Resolve variable names and then
    // Translate markdown -> HTML
    while (parse_tree->list->head != NULL)
    {
        queue_dequeue(parse_tree, &current_lexeme);
        
        if (current_lexeme)
        {
            if (istext(current_lexeme))
            {
                queue_enqueue(sema->compiled_parse_tree, &current_lexeme);
            }
            else
            {
                str_tolower(current_lexeme, strlen(current_lexeme));
                 
                entry = (struct lexeme_to_html_translation_entry **) ht_find(lexeme_lookup_tb, current_lexeme); 
                
                /*
                // Update our current scope level as needed
                if (strncmp(current_lexeme, "{", 1) == 0)
                {
                    *var_scope_level++;
                }
                else if (strncmp(current_lexeme, "}", 1) == 0)
                {
                    // Delete variables from symbol_table with current scope level
                    
                    // Dec scope level 
                    *var_scope_level--;
                }
                */

                if (entry) 
                {
                    current_conversion_entry = *entry;
                    current_conversion_entry->func(sema, current_conversion_entry->first_func_arg, current_lexeme);
                    free(current_lexeme);
                }
                else
                {
                    printf("Func for %s not defined\n", current_lexeme);
                    queue_enqueue(sema->compiled_parse_tree, &current_lexeme);
                }
            }
        }
        else
        {
            fprintf(stderr, "ERROR: Emptry string found\n");
            retval = SEM_FAILURE;
            break;
        }
    }

    // Write our new HTML queue to the file
    
    return retval;
}


static void init_lookup_table(hashtable_t **lookup_tb)
{
    struct lexeme_to_html_translation_entry *tmp = NULL;
    int i;

    *lookup_tb = ht_create_new(100, sizeof(struct lexeme_to_html_translation_entry *), NULL);
    
    for (i = 0; i < SEM_LEXEME_COMPILE_SYMBOL_ARRAY_SIZE; i++)
    {
        if (lexeme_compile_lookup_table[i].func)
        {
            tmp = &lexeme_compile_lookup_table[i];
            ht_insert(*lookup_tb, (char *) lexeme_compile_lookup_table[i].lexeme_markdown_entry.str, &tmp);
        }
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
