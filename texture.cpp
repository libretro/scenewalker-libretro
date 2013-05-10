#include "texture.hpp"
#include "rpng.h"
#include <cstdlib>
#include <cstdint>

namespace GL
{
   Texture::Texture(const std::string& path) : tex(0)
   {
      std::uint32_t* data = nullptr;
      unsigned width = 0, height = 0;

      bool ret = rpng_load_image_argb(path.c_str(),
            &data, &width, &height);

      if (ret)
      {
         SYM(glGenTextures)(1, &tex);
         bind();
         
#ifdef GLES
#if defined(GL_BGRA) && !defined(GL_BGRA_EXT)
#define GL_BGRA_EXT GL_BGRA
#endif
         SYM(glTexImage2D)(GL_TEXTURE_2D,
               0, GL_BGRA_EXT, width, height, 0,
               GL_BGRA_EXT, GL_UNSIGNED_BYTE,
               data);
#else
         SYM(glTexImage2D)(GL_TEXTURE_2D,
               0, GL_RGBA, width, height, 0,
               GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV,
               data);
#endif

         SYM(glGenerateMipmap)(GL_TEXTURE_2D);
         SYM(glTexParameteri)(GL_TEXTURE_2D,
               GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
         SYM(glTexParameteri)(GL_TEXTURE_2D,
               GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

         unbind();
         std::free(data);
      }
      else
         std::cerr << "Failed to load image: " << path << std::endl;
   }

   Texture::~Texture()
   {
      if (dead_state)
         return;

      SYM(glDeleteTextures)(1, &tex);
   }

   void Texture::bind(unsigned unit)
   {
      SYM(glActiveTexture)(GL_TEXTURE0 + unit);
      SYM(glBindTexture)(GL_TEXTURE_2D, tex);
      SYM(glActiveTexture)(GL_TEXTURE0);
   }

   void Texture::unbind(unsigned unit)
   {
      SYM(glActiveTexture)(GL_TEXTURE0 + unit);
      SYM(glBindTexture)(GL_TEXTURE_2D, 0);
      SYM(glActiveTexture)(GL_TEXTURE0);
   }
}

