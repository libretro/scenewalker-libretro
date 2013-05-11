#ifndef UTIL_HPP__
#define UTIL_HPP__

#include <string>
#include <cstdlib>

#define DIR_BACK(string) (string[string.length()-1])

namespace Path
{
   inline std::string basedir(const std::string& path)
   {
      size_t last = path.find_last_of("/\\");
      if (last != std::string::npos)
         return path.substr(0, last);
      else
         return ".";
   }

   inline std::string join(const std::string& dir, const std::string& path)
   {
      char last = dir.size() ? DIR_BACK(dir) : '\0';
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

   inline int stoi(const std::string& str)
   {
      return std::strtol(str.c_str(), NULL, 0);
   }

   inline float stof(const std::string& str)
   {
      return static_cast<float>(std::strtod(str.c_str(), NULL));
   }
}

#endif

