#ifndef _SHARED_H
#define _SHARED_H

#ifndef __cplusplus
#include <stdbool.h>
#endif

#ifdef _WIN32
#include <memory>
#define snprintf _snprintf
#else
#include <tr1/memory>
#endif

void retro_stderr(const char *str);

#endif
