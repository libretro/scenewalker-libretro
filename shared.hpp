#ifndef SHARED_HPP__
#define SHARED_HPP__

#ifdef _MSC_VER
#include <memory>
#define snprintf _snprintf
#else
#include <tr1/memory>
#endif

#ifdef __QNX__
namespace std1 = compat;
#else
namespace std1 = std::tr1;
#endif

void retro_stderr(const char *str);

#endif

