#ifndef UTIL_HPP__
#define UTIL_HPP__

#include <string>
#include <cstdlib>

namespace Path
{
   inline std::string basedir(const std::string& path)
   {
      auto last = path.find_last_of("/\\");
      if (last != std::string::npos)
         return path.substr(0, last);
      else
         return ".";
   }

   inline std::string join(const std::string& dir, const std::string& path)
   {
      char last = dir.size() ? dir.back() : '\0';
      std::string sep;
      if (last != '/' && last != '\\')
         sep = "/";
      return dir + sep + path;
   }
}

namespace String
{
   inline std::vector<std::string> split(const std::string& str, const std::string& splitter, bool keep_empty = false)
   {
      std::vector<std::string> list;

      for (std::size_t pos = 0, endpos = 0;
            endpos != std::string::npos; pos = endpos + 1)
      {
         endpos = str.find_first_of(splitter, pos);

         if (endpos - pos)
            list.push_back(str.substr(pos, endpos - pos));
         else if (keep_empty)
            list.push_back("");
      }

      return list;
   }

   // GCC 4.6 doesn't have std::stoi/std::stof ...
#define GCC_VERSION (__GNUC__ * 10000 + __GNUC__MINOR__ * 100)
   inline int stoi(const std::string& str)
   {
#if defined(__GNUC__) && (GCC_VERSION < 40700)
      return std::strtol(str.c_str(), nullptr, 0);
#else
      return std::stoi(str);
#endif
   }

   inline float stof(const std::string& str)
   {
#if defined(__GNUC__) && (GCC_VERSION < 40700)
      return std::strtod(str.c_str(), nullptr);
#else
      return std::stof(str);
#endif
   }
}


#endif

