#include "object.hpp"
#include "util.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace GL;
using namespace glm;
using namespace std;

namespace OBJ
{
   template<typename T>
   inline T parse_line(const string& data);

   template<>
   inline vec2 parse_line(const string& data)
   {
      float x = 0, y = 0;
      auto split = String::split(data, " ");
      if (split.size() >= 2)
      {
         x = stof(split[0]);
         y = stof(split[1]);
      }

      return vec2(x, y);
   }

   template<>
   inline vec3 parse_line(const string& data)
   {
      float x = 0, y = 0, z = 0;
      auto split = String::split(data, " ");
      if (split.size() >= 3)
      {
         x = stof(split[0]);
         y = stof(split[1]);
         z = stof(split[2]);
      }
      return vec3(x, y, z);
   }

   inline size_t translate_index(int index, size_t size)
   {
      return index < 0 ? size + index + 1 : index;
   }

   static void parse_vertex(const string& data,
         vector<Vertex>& vertices_buffer,
         const vector<vec3>& vertex,
         const vector<vec3>& normal,
         const vector<vec2>& tex)
   {
      auto vertices = String::split(data, " ");
      if (vertices.size() > 3)
         vertices.resize(3);

      vector<vector<string>> verts;
      for (auto& vert : vertices)
      {
         Vertex out_vertex{};

         auto coords = String::split(vert, "/", true);
         if (coords.size() == 1) // Vertex only
         {
            size_t coord = translate_index(stoi(coords[0]), vertex.size());

            if (coord && vertex.size() >= coord)
               out_vertex.vert = vertex[coord - 1];
         }
         else if (coords.size() == 2) // Vertex/Texcoord
         {
            size_t coord_vert = translate_index(stoi(coords[0]), vertex.size());
            size_t coord_tex  = translate_index(stoi(coords[1]), tex.size());

            if (coord_vert && vertex.size() >= coord_vert)
               out_vertex.vert = vertex[coord_vert - 1];
            if (coord_tex && tex.size() >= coord_tex)
               out_vertex.tex = tex[coord_tex - 1];
         }
         else if (coords.size() == 3 && coords[1].size()) // Vertex/Texcoord/Normal
         {
            size_t coord_vert   = translate_index(stoi(coords[0]), vertex.size());
            size_t coord_tex    = translate_index(stoi(coords[1]), tex.size());
            size_t coord_normal = translate_index(stoi(coords[2]), normal.size());

            if (coord_vert && vertex.size() >= coord_vert)
               out_vertex.vert = vertex[coord_vert - 1];
            if (coord_tex && tex.size() >= coord_tex)
               out_vertex.tex = tex[coord_tex - 1];
            if (coord_normal && normal.size() >= coord_normal)
               out_vertex.normal = normal[coord_normal - 1];
         }
         else if (coords.size() == 3 && !coords[1].size()) // Vertex//Normal
         {
            size_t coord_vert   = translate_index(stoi(coords[0]), vertex.size());
            size_t coord_normal = translate_index(stoi(coords[2]), normal.size());

            if (coord_vert && vertex.size() >= coord_vert)
               out_vertex.vert = vertex[coord_vert - 1];
            if (coord_normal && normal.size() >= coord_normal)
               out_vertex.normal = normal[coord_normal - 1];
         }

         vertices_buffer.push_back(out_vertex);
      }
   }

   vector<shared_ptr<Mesh>> load_from_file(const string& path)
   {
      ifstream file(path, ios::in);
      vector<shared_ptr<Mesh>> meshes;
      if (!file.is_open())
         return meshes;

      vector<vec3> vertex;
      vector<vec3> normal;
      vector<vec2> tex;

      vector<Vertex> vertices;

      // Texture cache.
      map<string, shared_ptr<Texture>> textures;
      shared_ptr<Texture> current_texture;

      for (string line; getline(file, line); )
      {
         line = line.substr(0, line.find_first_of('\r'));

         auto split_point = line.find_first_of(' ');
         auto type = line.substr(0, split_point);
         auto data = split_point != string::npos ? line.substr(split_point + 1) : string();

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
            if (vertices.size()) // Different texture, new mesh.
            {
               auto mesh = make_shared<Mesh>();
               mesh->set_vertices(move(vertices));
               mesh->set_texture(current_texture);
               meshes.push_back(mesh);
            }

            auto& texture = textures[data];
            if (!texture)
            {
               auto texture_path = Path::join(Path::basedir(path), data + ".png");
               texture = make_shared<Texture>(texture_path);
            }

            current_texture = texture;
         }
      }

      if (vertices.size())
      {
         auto mesh = make_shared<Mesh>();
         mesh->set_vertices(move(vertices));
         mesh->set_texture(current_texture);
         meshes.push_back(mesh);
      }

      return meshes;
   }
}

