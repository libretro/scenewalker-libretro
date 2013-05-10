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
         void set_vertices(const std::shared_ptr<std::vector<Vertex>>& vertex);
         void set_vertex_type(GLenum type);
         void set_texture(const std::shared_ptr<Texture>& tex);
         void set_shader(const std::shared_ptr<Shader>& shader);

         void set_model(const glm::mat4& model);
         void set_view(const glm::mat4& view);
         void set_projection(const glm::mat4& projection);

         void render();

      private:
         GLuint vbo;
         GLenum vertex_type;
         std::shared_ptr<std::vector<Vertex>> vertex;
         std::shared_ptr<Texture> texture;
         std::shared_ptr<Shader> shader;

         glm::mat4 model;
         glm::mat4 view;
         glm::mat4 projection;
         glm::mat4 mvp;
   };
}

#endif

