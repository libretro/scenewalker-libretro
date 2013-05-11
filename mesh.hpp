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
#include "shared.hpp"

namespace GL
{
   struct Vertex
   {
      glm::vec3 vert;
      glm::vec3 normal;
      glm::vec2 tex;
   };

   struct Material
   {
      Material() :
         ambient(1, 1, 1),
         diffuse(1, 1, 1)
      {}

      glm::vec3 ambient;
      glm::vec3 diffuse;
      std1::shared_ptr<Texture> diffuse_map;
   };

   class Mesh
   {
      public:
         Mesh();
         ~Mesh();

         void set_vertices(std::vector<Vertex> vertex);
         void set_vertices(const std1::shared_ptr<std::vector<Vertex> >& vertex);
         void set_vertex_type(GLenum type);
         void set_material(const Material& material);
         void set_shader(const std1::shared_ptr<Shader>& shader);

         void set_model(const glm::mat4& model);
         void set_view(const glm::mat4& view);
         void set_projection(const glm::mat4& projection);

         void set_light_dir(const glm::vec3& light_dir);
         void set_light_ambient(const glm::vec3& light_ambient);

         void render();

      private:
         GLuint vbo;
         GLenum vertex_type;
         std1::shared_ptr<std::vector<Vertex> > vertex;
         std1::shared_ptr<Shader> shader;

         Material material;
         glm::vec3 light_dir;
         glm::vec3 light_ambient;

         glm::mat4 model;
         glm::mat4 view;
         glm::mat4 projection;
         glm::mat4 mvp;
   };
}

#endif

