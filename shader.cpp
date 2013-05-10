#include "shader.hpp"

namespace GL
{
   Shader::Shader(const std::string& vertex_src, const std::string& fragment_src)
   {
      prog = SYM(glCreateProgram)();

      GLuint vertex = compile_shader(GL_VERTEX_SHADER, vertex_src);
      GLuint frag = compile_shader(GL_FRAGMENT_SHADER, fragment_src);

      SYM(glAttachShader)(prog, vertex);
      SYM(glAttachShader)(prog, frag);
      SYM(glLinkProgram)(prog);
   }

   GLuint Shader::compile_shader(GLenum type, const std::string& source)
   {
      GLuint shader = SYM(glCreateShader)(type);

      const char* src = source.c_str();
      SYM(glShaderSource)(shader, 1, &src, nullptr);
      SYM(glCompileShader)(shader);

      return shader;
   }

   Shader::~Shader()
   {
      // Don't release shader.
   }

   void Shader::use()
   {
      SYM(glUseProgram)(prog);
   }

   void Shader::unbind()
   {
      SYM(glUseProgram)(0);
   }

   GLint Shader::uniform(const char* sym)
   {
      GLint ret = -1;

      auto itr = map.find(sym);
      if (itr == std::end(map))
         map[sym] = ret = SYM(glGetUniformLocation)(prog, sym);
      else
         ret = itr->second;

      return ret;
   }

   GLint Shader::attrib(const char* sym)
   {
      GLint ret = -1;

      auto itr = map.find(sym);
      if (itr == std::end(map))
         map[sym] = ret = SYM(glGetAttribLocation)(prog, sym);
      else
         ret = itr->second;

      return ret;
   }
}

