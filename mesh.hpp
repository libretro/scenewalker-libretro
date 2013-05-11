#ifndef MESH_HPP__
#define MESH_HPP__

#include "gl.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <vector>
#include <cstddef>
#include <memory>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <tr1/memory>

namespace GL
{
   struct Vertex
   {
      glm::vec3 vert;
      glm::vec3 normal;
      glm::vec2 tex;
   };

   class Mesh
   {
      public:
         Mesh();
         ~Mesh();

         void set_vertices(std::vector<Vertex> vertex);
         void set_vertices(const std::tr1::shared_ptr<std::vector<Vertex> >& vertex);
         void set_vertex_type(GLenum type);
         void set_texture(const std::tr1::shared_ptr<Texture>& tex);
         void set_shader(const std::tr1::shared_ptr<Shader>& shader);

         void set_model(const glm::mat4& model);
         void set_view(const glm::mat4& view);
         void set_projection(const glm::mat4& projection);

         void render();

      private:
         GLuint vbo;
         GLenum vertex_type;
         std::tr1::shared_ptr<std::vector<Vertex> > vertex;
         std::tr1::shared_ptr<Texture> texture;
         std::tr1::shared_ptr<Shader> shader;

         glm::mat4 model;
         glm::mat4 view;
         glm::mat4 projection;
         glm::mat4 mvp;
   };
}

#endif

