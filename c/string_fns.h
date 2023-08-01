#ifndef STRING_LIBRARY_FNS_H
#define STRING_LIBRARY_FNS_H

#include <stdlib.h>

size_t slength(const char *str);
char  *scopy(const char *str);
char  *sformat(const char *fmt, ...);
char  *supper(const char *str);
char  *slower(const char *str);
char  *sreverse(const char *str);
char  *scapitalize(const char *str);

#endif // STRING_LIBRARY_FNS_H