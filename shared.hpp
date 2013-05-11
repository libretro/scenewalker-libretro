#ifndef SHARED_HPP__
#define SHARED_HPP__

#ifdef _MSC_VER
#include <memory>
#define snprintf _snprintf
#else
#include <tr1/memory>
#endif

void retro_stderr(const char *str);

#endif

