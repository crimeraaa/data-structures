/**
 * @file namespace_string.c
 * @brief Implementing a sort of "namespace" in C, just for fun.
 * This "namespace" is based off of the Lua string library.
 * This is not meant to be actually used. It's purely for my own experimentation.
 */

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>

// Use this to collect all our strings in a linked list so we can free them all.
#include "stack-linkedlist/genstack_sllist.h"

// Scoped to this file, need only this Stack* and most fn's need to poke at it
// Don't want the user poking at it from <main>.
Stack *storedstrings = NULL;

// Forward decl's in here so we don't pollute other translation units
// with common names like "copy", "length", "format", "upper", etc.
#include "string_fns.h"
#include "namespace_string.h"

const struct NamespaceString stringlib =
{
    .copy       = scopy,
    .length     = slength,
    .format     = sformat,
    .upper      = supper,
    .lower      = slower,
    .reverse    = sreverse,
    .capitalize = scapitalize,
};

static void print_stored_string(void *obj)
{
    printf("'%s'\n", (char*)obj);
}

static char *storage_insert(char *str)
{
    if (storedstrings)
    {
        if (!stack_push(storedstrings, str))
        {
            // We couldn't allocate space in the stack somehow, too bad
            free(str);
            return NULL;
        }
        // Implied else: we successfully pushed to the stack!
        return str;
    }

    // Implied else: <stored> == NULL
    storedstrings = stack_init(str, print_stored_string, NULL);

    // stack_init failed and return <NULL>, so too bad
    if (!storedstrings) 
    {
        free(str);
        return NULL;
    }

    // Implied else: we successfully initialised <stored>
    return str;
}

char *get_string(const char *prompt, ...)
{
    va_list argp;
    va_start(argp, prompt);
    vprintf(prompt, argp);
    va_end(argp);

    // Greater value = less realloc, more wasted memory, and vice versa.
    size_t increment = 8;

    // Also arbitrary, change this to any value you deem fit.
    size_t capacity = 16;
    
    // Current maximum allowable index we can write to, meant to update frequently.
    size_t max_idx = capacity - 1;

    char *str = malloc(capacity * sizeof(char));
    if (!str)
    {
        printf("Failed to allocate memory for input string!\n");
        return NULL;
    }

    // Use the (int) type so we can check for <EOF> ( -1).
    int c;
    size_t idx = 0;
    while ( (c = fgetc(stdin)) != EOF && c != '\r' && c != '\n')
    {
        // We reached our current max idx, so prepare to realloc.
        if (idx > max_idx)
        {
            capacity += increment;
            max_idx += increment;

            // Original memory block is not freed if realloc fails.
            char *tmp = str;
            tmp = realloc(str, capacity);
            if (tmp == NULL)
            {
                printf("Failed to reallocate memory for input string!\n");
                free(str);
                return NULL;
            }
            
            // Original memory block is freed if realloc succeeds.
            str = tmp;
        }

        // assignment occurs first, then idx is incremented.
        str[idx++] = c;
    }

    // Windows line ending is CRLF (\r\n), so check for that
    if (c == '\r')
    {
        c = fgetc(stdin);
        // Some systems use CR (\r) as is, so account for that.
        if (c != '\n') ungetc(c, stdin);
    }

    str[idx] = '\0';
    return str;
}

void print_all_strings(void)
{
    // Wrapper because user cannot access (Stack *) <stored>.
    // User is NOT meant to access <stored>.
    stack_print(storedstrings);
}

void clear_all_strings(void)
{
    // Same case as <print_all_strings>.
    stack_destroy(&storedstrings);
}

size_t slength(const char *str)
{
    // <*str> means get the first char.
    // <*str> == '\0' is the same as saying "first char is 0/false".
    if (!str || !*str) return 0;

    size_t count = 0;

    // Idiom to check the string's current char if it's anything but 0.
    while (*str)
    {
        count++;
        str++;
    }

    return count;
}

char *scopy(const char *str)
{
    size_t nul_idx = slength(str);

    char *ret = calloc(nul_idx, sizeof(char));
    if (!ret) return NULL;

    for (size_t i = 0; i < nul_idx; i++)
        ret[i] = str[i];

    ret[nul_idx] = '\0';

    return storage_insert(ret);
}


char *sformat(const char *fmt, ...)
{
    va_list argp;
    va_start(argp, fmt);

    va_list argp2;
    va_copy(argp2, argp);

    int num_chars = vsnprintf(NULL, 0, fmt, argp);
    va_end(argp);
    if (num_chars < 0)
    {
        // Failed to get length of resulting formatted string
        va_end(argp2);
        return NULL;
    }
    // Add 1 for nul char (wasn't part of original count)
    num_chars++;

    char *str = malloc(num_chars * sizeof(char));
    if (!str)
    {
        va_end(argp2);
        return NULL;
    }

    int status = vsnprintf(str, num_chars, fmt, argp2);
    va_end(argp2);
    if (status < 0 || status >= num_chars)
    {
        // Failed to write to format string somehow
        free(str);
        return NULL;
    }
    str[num_chars - 1] = '\0';
    
    return storage_insert(str);
}

char *supper(const char *str)
{
    size_t nul_idx = slength(str);
    char *ret = calloc(nul_idx + 1, sizeof(char));
    if (!ret) return NULL;
    
    for (size_t i = 0; i < nul_idx; i++)
        ret[i] = toupper(str[i]);

    ret[nul_idx] = '\0';
    return storage_insert(ret);
}

char *slower(const char *str)
{
    size_t nul_idx = slength(str);
    char *ret = calloc(nul_idx + 1, sizeof(char));
    if (!ret) return NULL;

    for (size_t i = 0; i < nul_idx; i++)
        ret[i] = tolower(str[i]);

    return storage_insert(ret);
}
char *sreverse(const char *str)
{
    size_t nul_idx = slength(str);
    char *ret = calloc(nul_idx + 1, sizeof(char));
    if (!ret) return NULL;

    size_t i = 0;
    // Subtract 1 so we don't copy nul char to the first index of ret
    size_t j = nul_idx - 1;
    while (i < nul_idx)
    {
        ret[i] = str[j];
        i++;
        j--;
    }
    ret[nul_idx] = '\0';
    return storage_insert(ret);
}

char *scapitalize(const char *str)
{
    size_t nul_idx = slength(str);

    char *ret = calloc(nul_idx, sizeof(char));
    if (!ret) return NULL;

    for (size_t i = 0; i < nul_idx; i++)
        ret[i] = str[i];

    ret[nul_idx] = '\0';
    ret[0] = toupper(ret[0]);
    return storage_insert(ret);
}