#ifndef SHADER_HPP__
#define SHADER_HPP__

#include "gl.hpp"

namespace GL
{
   class Shader
   {
      public:
         Shader(const std::string& vertex, const std::string& fragment);
         ~Shader();
         void use();

         static void unbind();

         GLint uniform(const char* sym);
         GLint attrib(const char* sym);

      private:
         GLuint prog;
         std::map<std::string, GLint> map;

         GLuint compile_shader(GLenum type, const std::string& source);
   };
}

#endif

