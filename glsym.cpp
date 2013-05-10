#include "gl.hpp"

namespace GL
{
   static SymMap map;
   SymMap& symbol_map()
   {
      return map;
   }

   void init_symbol_map()
   {
      map.clear();
   }

   static retro_hw_get_proc_address_t proc;
   void set_function_cb(retro_hw_get_proc_address_t proc_)
   {
      proc = proc_;
   }

   retro_proc_address_t get_symbol(const std::string& str)
   {
      return proc(str.c_str());
   }
}

