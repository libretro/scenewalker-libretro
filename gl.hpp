#ifndef GL_HPP__
#define GL_HPP__

#define GL_GLEXT_PROTOTYPES
#if defined(GLES)
#include <GLES2/gl2.h>
#else
#include <GL/gl.h>
#include <GL/glext.h>
#endif

#include <map>
#include <iostream>
#include "libretro.h"

#define SYM(sym) (::GL::symbol<decltype(&sym)>(#sym))

namespace GL
{
   typedef std::map<std::string, retro_proc_address_t> SymMap;

   SymMap& symbol_map();
   void init_symbol_map();

   void set_function_cb(retro_hw_get_proc_address_t);
   retro_proc_address_t get_symbol(const std::string& str);

   template<typename Func>
   inline Func symbol(const std::string& sym)
   {
      auto& map = symbol_map();
      
      auto& func = map[sym];
      if (!func)
      {
         func = get_symbol(sym);
         if (!func)
            std::cerr << "Didn't find GL symbol: " << sym << std::endl;
      }

      return reinterpret_cast<Func>(func);
   }
}

#endif

