#include "object.hpp"
#include "util.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace GL;
using namespace glm;

namespace OBJ
{
   template<typename T>
   inline T parse_line(const std::string& data);

   template<>
   inline vec2 parse_line(const std::string& data)
   {
      float x = 0, y = 0;
      auto split = String::split(data, " ");
      if (split.size() >= 2)
      {
         x = std::stof(split[0]);
         y = std::stof(split[1]);
      }

      return vec2(x, y);
   }

   template<>
   inline vec3 parse_line(const std::string& data)
   {
      float x = 0, y = 0, z = 0;
      auto split = String::split(data, " ");
      if (split.size() >= 3)
      {
         x = std::stof(split[0]);
         y = std::stof(split[1]);
         z = std::stof(split[2]);
      }
      return vec3(x, y, z);
   }

   static void parse_vertex(const std::string& data,
         std::vector<Vertex>& vertices_buffer,
         const std::vector<vec3>& vertex,
         const std::vector<vec3>& normal,
         const std::vector<vec2>& tex)
   {
      auto vertices = String::split(data, " ");
      if (vertices.size() > 3)
         vertices.resize(3);

      std::vector<std::vector<std::string>> verts;
      for (auto& vert : vertices)
      {
         Vertex out_vertex{};

         auto coords = String::split(vert, "/", true);
         if (coords.size() == 1) // Vertex only
         {
            std::size_t coord = std::stoi(coords[0]);
            if (coord && vertex.size() >= coord)
               out_vertex.vert = vertex[coord - 1];
         }
         else if (coords.size() == 2) // Vertex/Texcoord
         {
            std::size_t coord_vert = std::stoi(coords[0]);
            std::size_t coord_tex  = std::stoi(coords[1]);
            if (coord_vert && vertex.size() >= coord_vert)
               out_vertex.vert = vertex[coord_vert - 1];
            if (coord_tex && tex.size() >= coord_tex)
               out_vertex.tex = tex[coord_tex - 1];
         }
         else if (coords.size() == 3 && coords[1].size()) // Vertex/Texcoord/Normal
         {
            std::size_t coord_vert   = std::stoi(coords[0]);
            std::size_t coord_tex    = std::stoi(coords[1]);
            std::size_t coord_normal = std::stoi(coords[2]);
            if (coord_vert && vertex.size() >= coord_vert)
               out_vertex.vert = vertex[coord_vert - 1];
            if (coord_tex && tex.size() >= coord_tex)
               out_vertex.tex = tex[coord_tex - 1];
            if (coord_normal && normal.size() >= coord_normal)
               out_vertex.normal = normal[coord_normal - 1];
         }
         else if (coords.size() == 3 && !coords[1].size()) // Vertex//Normal
         {
            std::size_t coord_vert   = std::stoi(coords[0]);
            std::size_t coord_normal = std::stoi(coords[2]);
            if (coord_vert && vertex.size() >= coord_vert)
               out_vertex.vert = vertex[coord_vert - 1];
            if (coord_normal && normal.size() >= coord_normal)
               out_vertex.normal = normal[coord_normal - 1];
         }

         vertices_buffer.push_back(out_vertex);
      }
   }

   std::vector<std::shared_ptr<Mesh>> load_from_file(const std::string& path)
   {
      std::ifstream file(path, std::ios::in);
      std::vector<std::shared_ptr<Mesh>> meshes;
      if (!file.is_open())
         return meshes;

      std::vector<vec3> vertex;
      std::vector<vec3> normal;
      std::vector<vec2> tex;

      std::vector<Vertex> vertices;

      std::shared_ptr<Texture> texture;

      for (std::string line; getline(file, line); )
      {
         line = line.substr(0, line.find_first_of('\r'));

         auto split_point = line.find_first_of(' ');
         auto type = line.substr(0, split_point);
         auto data = split_point != std::string::npos ? line.substr(split_point + 1) : std::string();

         if (type == "v")
            vertex.push_back(parse_line<vec3>(data));
         else if (type == "vn")
            normal.push_back(parse_line<vec3>(data));
         else if (type == "vt")
            tex.push_back(parse_line<vec2>(data));
         else if (type == "f")
            parse_vertex(data, vertices, vertex, normal, tex);
         else if (type == "texture") // Not standard OBJ, but do it like this for simplicity ...
         {
            auto texture_path = Path::join(Path::basedir(path), data + ".png");
            texture = std::make_shared<Texture>(texture_path);
         }
      }

      std::cerr << "Got " << vertices.size() << " vertices!" << std::endl;

      auto mesh = std::make_shared<Mesh>();
      mesh->set_vertices(std::move(vertices));
      mesh->set_texture(texture);

      meshes.push_back(mesh);
      return meshes;
   }
}

