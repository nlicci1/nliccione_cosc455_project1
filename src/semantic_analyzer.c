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

// This function is a simple helper function for LINK
// This skips over the next symbol frees the memory allocated for it,
// gets the next symbol (this will be the retval) and then
// gets 1 more symbol from the parse_tree and frees the memory for it.
static char *sem_get_content_from_ptree(queue *parse_tree)
{
    char *retval = NULL;
    char *tmp = NULL;

    queue_dequeue(parse_tree, &tmp);

    if (tmp)
    {
        free(tmp);
        queue_dequeue(parse_tree, &tmp);
        
        if (tmp)
        {
            retval = tmp;
            queue_dequeue(parse_tree, &tmp);
            
            if (tmp)
            {
                free(tmp);
            }
        }
    }

    return retval;
}

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

// Implements the semantics for AUDIO and VIDEO 
// In order for this function to work properly the compiled_lexeme_format parameter
// must have a snprintf like 'format' style in order to copy the url into the link portion
// of the compiled string. 
static int sem_compile_resource_locators(sem_t *semantic_analyzer, const char *compiled_lexeme_format, char *markdown_lexeme)
{
    queue *parse_tree = NULL;
    char *url = NULL;
    char *compiled_lexeme = NULL;
    size_t new_comp_str_len;
    size_t url_len;
    
    parse_tree = semantic_analyzer->markdown_parse_tree;
    if (compiled_lexeme_format && markdown_lexeme && parse_tree)
    {
        url = sem_get_content_from_ptree(parse_tree);

        if (!url)
        {
            // This shouldnt happen this is a syntax error!
            return SEM_FAILURE; 
        }
        
        url_len = strlen(url);
        new_comp_str_len = strlen(compiled_lexeme_format) + url_len + 1;

        if ((compiled_lexeme = malloc(new_comp_str_len)))
        {
            memset(compiled_lexeme, '\0', new_comp_str_len);
            snprintf(compiled_lexeme, new_comp_str_len - 1, compiled_lexeme_format, url); 
            queue_enqueue(semantic_analyzer->compiled_parse_tree, &compiled_lexeme);
            free(url);

            return SEM_SUCCESS;
        }

        return SEM_FAILURE;
    }
    else
    {
        fprintf(stderr, "Error: Compile failed for %s. Could not be translated to HTML\n", markdown_lexeme);
        return SEM_FAILURE;
    }
}

// [ txt ] ( txt )
// { { LINKB, "[" }, sem_compile_link, "<a href=\"%s\">%s</a>" },
static int sem_compile_link(sem_t *semantic_analyzer, const char *compiled_lexeme_format, char *markdown_lexeme)
{
    queue *parse_tree = NULL;
    char *compiled_lexeme = NULL;
    char *current_lexeme = NULL;
    char *link_name = NULL;
    char *url = NULL;
    size_t new_comp_strlen;
    
    parse_tree = semantic_analyzer->markdown_parse_tree; 
    
    // Get link name
    queue_dequeue(parse_tree, &current_lexeme);
    link_name = current_lexeme;
    new_comp_strlen  = strlen(link_name);

    // Get ending ']' tag and free the memory for that string
    queue_dequeue(parse_tree, &current_lexeme);
    free(current_lexeme);
    
    // Get URL string
    url = sem_get_content_from_ptree(parse_tree);
    new_comp_strlen += strlen(url) + strlen(compiled_lexeme_format) + 1;
    
    if ((compiled_lexeme = malloc(new_comp_strlen)))
    {
        memset(compiled_lexeme, '\0', new_comp_strlen);
        snprintf(compiled_lexeme, new_comp_strlen - 1, compiled_lexeme_format, url, link_name); 
        queue_enqueue(semantic_analyzer->compiled_parse_tree, &compiled_lexeme);
        
        free(url);
        free(link_name);

        return SEM_SUCCESS;
    }

    return SEM_FAILURE;
}

static int sem_compile_head(sem_t *semantic_analyzer, const char *compiled_lexeme, char *markdown_lexeme)
{
    struct lexeme_to_html_translation_entry **entry = NULL;
    struct lexeme_to_html_translation_entry *current_conversion_entry = NULL;
    queue *parse_tree = NULL;
    hashtable_t *lexeme_lookup_tb = NULL;
    char *current_lexeme = NULL;
    char *tmp = NULL;
    int i;
    
    lexeme_lookup_tb = (hashtable_t *) semantic_analyzer->lexeme_lookup_table;
    parse_tree = semantic_analyzer->markdown_parse_tree; 

    // Add <head> to compiled ptree
    tmp = strdup(compiled_lexeme);
    queue_enqueue(semantic_analyzer->compiled_parse_tree, &tmp);

    for (i = 0; i < 3; i++)
    {
        queue_dequeue(parse_tree, &current_lexeme);
        if (current_lexeme)
        {
            if (istext(current_lexeme))
            {
                queue_enqueue(semantic_analyzer->compiled_parse_tree, &current_lexeme);
            }
            else
            {
                str_tolower(current_lexeme, strlen(current_lexeme));
                entry = (struct lexeme_to_html_translation_entry **) ht_find(lexeme_lookup_tb, current_lexeme); 
                
                if (entry) 
                {
                    current_conversion_entry = *entry;
                    current_conversion_entry->func(semantic_analyzer, current_conversion_entry->first_func_arg, current_lexeme);
                    free(current_lexeme);
                }
            }
        }
    }

    // Add </head> to compiled ptree
    tmp = strdup("</head>");
    queue_enqueue(semantic_analyzer->compiled_parse_tree, &tmp);
    queue_dequeue(parse_tree, &current_lexeme);
    free(current_lexeme);

    return SEM_SUCCESS;
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
    // The %s will be used to easily place the URL inside the link
    { { LINKB, "[" }, sem_compile_link, "<a href=\"%s\">%s</a>" },
    { { AUDIO, "@" }, sem_compile_resource_locators, "<audio controls> <source src=\"%s\"> </audio>" },
    { { VIDEO, "%" }, sem_compile_resource_locators, "<iframe src=\"%s\"/></iframe>" },
    { { VAR_LEXEME, "$def" }, NULL, NULL },
    { { VAR_LEXEME, "$use" }, NULL, NULL },
    { { HEAD, "^" }, sem_compile_head, "<head>" },
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
    //unsigned int *var_scope_level = &sema->variable_scope_level;
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
                    //printf("Func for %s not defined\n", current_lexeme);
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

    *lookup_tb = ht_create_new(256, sizeof(struct lexeme_to_html_translation_entry *), NULL);
    
    for (i = 0; i < SEM_LEXEME_COMPILE_SYMBOL_ARRAY_SIZE; i++)
    {
        if (lexeme_compile_lookup_table[i].func)
        {
            tmp = &lexeme_compile_lookup_table[i];
            ht_insert(*lookup_tb, (char *) lexeme_compile_lookup_table[i].lexeme_markdown_entry.str, &tmp);
        }
    }
}

static void compiled_parse_tree_entry_element_free(void *ele)
{
    if (ele)
    {
        free(* (char **) ele);
    }
}

void SEM_create_new(sem_t *sem, queue *parse_tree, char *html_file_name)
{
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
        queue_new(sem->compiled_parse_tree, sizeof(char *), compiled_parse_tree_entry_element_free);
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
