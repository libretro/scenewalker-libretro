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
         ambient(0, 0, 0),
         diffuse(0, 0, 0),
         specular(0, 0, 0),
         specular_power(60.0),
         alpha_mod(1.0f)
      {}

      glm::vec3 ambient;
      glm::vec3 diffuse;
      glm::vec3 specular;
      float specular_power;
      float alpha_mod;
      std1::shared_ptr<Texture> diffuse_map;
      std1::shared_ptr<Texture> ambient_map;
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
         void set_blank(const std1::shared_ptr<Texture>& blank);
         void set_shader(const std1::shared_ptr<Shader>& shader);

         void set_model(const glm::mat4& model);
         void set_view(const glm::mat4& view);
         void set_projection(const glm::mat4& projection);
         void set_eye(const glm::vec3& eye_pos);

         void set_light_pos(const glm::vec3& light_pos);
         void set_light_ambient(const glm::vec3& light_ambient);

         void render();

      private:
         GLuint vbo;
         GLenum vertex_type;
         std1::shared_ptr<std::vector<Vertex> > vertex;
         std1::shared_ptr<Shader> shader;
         std1::shared_ptr<Texture> blank;

         Material material;
         glm::vec3 light_pos;
         glm::vec3 light_ambient;
         glm::vec3 eye_pos;

         glm::mat4 model;
         glm::mat4 view;
         glm::mat4 projection;
         glm::mat4 mvp;
   };
}

#endif

