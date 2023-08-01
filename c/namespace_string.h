#ifndef STRING_NAMESPACE_H
#define STRING_NAMESPACE_H

#include <stdlib.h>

typedef char   *basic_fn(const char *str);
typedef char  *format_fn(const char *fmt, ...);
typedef size_t length_fn(const char *str);

struct NamespaceString
{
    basic_fn  *copy;
    length_fn *length;
    format_fn *format;
    basic_fn  *upper;
    basic_fn  *lower;
    basic_fn  *reverse;
    basic_fn  *capitalize;
};

char *get_string(const char *prompt, ...);
void print_all_strings(void);
void clear_all_strings(void);

// extern const struct so its members are defined only once in the .c file.
extern const struct NamespaceString stringlib;

#endif // STRING_NAMESPACE_H