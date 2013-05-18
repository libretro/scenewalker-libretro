#ifndef SHARED_HPP__
#define SHARED_HPP__

#ifdef _MSC_VER
#include <memory>
#define snprintf _snprintf
#else
#include <tr1/memory>
#endif

#if defined(__QNX__) || defined(__CELLOS_LV2__)
namespace std1 = compat;
#else
namespace std1 = std::tr1;
#endif

void retro_stderr(const char *str);
void retro_stderr_print(const char *fmt, ...);

#endif

