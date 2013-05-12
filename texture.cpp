#include "texture.hpp"
#include "rpng.h"
#include <stdint.h>
#include <stdlib.h>

using namespace std;
using namespace std1;

namespace GL
{
   Texture::Texture() : tex(0)
   {}

   void Texture::upload_data(const void* data, unsigned width, unsigned height,
         bool generate_mipmap)
   {
      if (!tex)
         SYM(glGenTextures)(1, &tex);

      bind();

      SYM(glTexImage2D)(GL_TEXTURE_2D,
            0, GL_RGBA, width, height, 0,
            GL_RGBA, GL_UNSIGNED_BYTE,
            data);

      if (generate_mipmap)
      {
         SYM(glGenerateMipmap)(GL_TEXTURE_2D);
         SYM(glTexParameteri)(GL_TEXTURE_2D,
               GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
         SYM(glTexParameteri)(GL_TEXTURE_2D,
               GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
      }
      else
      {
         SYM(glTexParameteri)(GL_TEXTURE_2D,
               GL_TEXTURE_MAG_FILTER, GL_LINEAR);
         SYM(glTexParameteri)(GL_TEXTURE_2D,
               GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      }

      unbind();
   }

   Texture::Texture(const std::string& path) : tex(0)
   {
      uint8_t* data = NULL;
      unsigned width = 0, height = 0;

      bool ret = rpng_load_image_rgba(path.c_str(),
            &data, &width, &height);

      if (ret)
      {
         upload_data(data, width, height, true);
         free(data);
      }
      else
         retro_stderr_print("Failed to load image: %s\n", path.c_str());
   }

   Texture::~Texture()
   {
      if (dead_state)
         return;

      if (tex)
         SYM(glDeleteTextures)(1, &tex);
   }

   void Texture::bind(unsigned unit)
   {
      SYM(glActiveTexture)(GL_TEXTURE0 + unit);
      SYM(glBindTexture)(GL_TEXTURE_2D, tex);
      SYM(glActiveTexture)(GL_TEXTURE0);
   }

   shared_ptr<Texture> Texture::blank()
   {
      shared_ptr<Texture> tex(new Texture);
      uint32_t data = -1;
      tex->upload_data(&data, 1, 1, false);
      return tex;
   }

   void Texture::unbind(unsigned unit)
   {
      SYM(glActiveTexture)(GL_TEXTURE0 + unit);
      SYM(glBindTexture)(GL_TEXTURE_2D, 0);
      SYM(glActiveTexture)(GL_TEXTURE0);
   }
}

