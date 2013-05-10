#ifndef TEXTURE_HPP__
#define TEXTURE_HPP__

#include "gl.hpp"

namespace GL
{
   class Texture
   {
      public:
         Texture(const std::string& path);
         ~Texture();

         void bind(unsigned unit = 0);
         static void unbind(unsigned unit = 0);

      private:
         GLuint tex;
   };
}

#endif

