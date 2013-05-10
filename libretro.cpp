#include "libretro.h"
#include "gl.hpp"
#include "mesh.hpp"
#include <cstring>
#include <string>
#include <iostream>

using namespace GL;
using namespace glm;

static struct retro_hw_render_callback hw_render;
static std::string mesh_path;

static std::shared_ptr<Mesh> mesh;
static std::shared_ptr<Texture> texture;
static std::shared_ptr<Shader> shader;

void retro_init(void)
{}

void retro_deinit(void)
{}

unsigned retro_api_version(void)
{
   return RETRO_API_VERSION;
}

void retro_set_controller_port_device(unsigned, unsigned)
{}

void retro_get_system_info(struct retro_system_info *info)
{
   std::memset(info, 0, sizeof(*info));
   info->library_name     = "ModelViewer";
   info->library_version  = "v1";
   info->need_fullpath    = true;
   info->valid_extensions = "obj";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   info->timing.fps = 60.0;
   info->timing.sample_rate = 30000.0;

   info->geometry.base_width  = 640;
   info->geometry.base_height = 480;
   info->geometry.max_width   = 640;
   info->geometry.max_height  = 480;
   info->geometry.aspect_ratio = 4.0 / 3.0;
}

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;
}

void retro_set_audio_sample(retro_audio_sample_t cb)
{
   audio_cb = cb;
}

void retro_set_audio_sample_batch(retro_audio_sample_batch_t cb)
{
   audio_batch_cb = cb;
}

void retro_set_input_poll(retro_input_poll_t cb)
{
   input_poll_cb = cb;
}

void retro_set_input_state(retro_input_state_t cb)
{
   input_state_cb = cb;
}

void retro_set_video_refresh(retro_video_refresh_t cb)
{
   video_cb = cb;
}

void retro_run(void)
{
   input_poll_cb();

   SYM(glBindFramebuffer)(GL_FRAMEBUFFER, hw_render.get_current_framebuffer());
   SYM(glClearColor)(0.2f, 0.2f, 0.2f, 1.0f);
   SYM(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   SYM(glEnable)(GL_DEPTH_TEST);

   SYM(glViewport)(0, 0, 640, 480);

   mesh->render();

   video_cb(RETRO_HW_FRAME_BUFFER_VALID, 640, 480, 0);
}

static void init_mesh(const std::string&)
{
   std::vector<Vertex> triangle = {
      {
         { -0.5, -0.5, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.0, 0.0 }
      },
      {
         { +0.5, -0.5, 0.0 }, { 0.0, 0.0, 1.0 }, { 1.0, 0.0 }
      },
      {
         { +0.0, +0.5, 0.0 }, { 0.0, 0.0, 1.0 }, { 0.5, 1.0 }
      },
   };

   mesh = std::make_shared<Mesh>();
   mesh->set_vertices(std::move(triangle));

   static const std::string vertex_shader =
      "uniform mat4 uModel;\n"
      "uniform mat4 uMVP;\n"
      "attribute vec4 aVertex;\n"
      "attribute vec3 aNormal;\n"
      "attribute vec2 aTex;\n"
      "varying vec4 vNormal;\n"
      "varying vec2 vTex;\n"
      "void main() {\n"
      "  gl_Position = uMVP * aVertex;\n"
      "  vTex = aTex;\n"
      "  vNormal = uModel * vec4(aNormal, 0.0);\n"
      "}";

   static const std::string fragment_shader =
      "varying vec2 vTex;\n"
      "varying vec4 vNormal;\n"
      "void main() {\n"
      "  gl_FragColor = vec4(0.5);\n"
      "}";

   shader = std::make_shared<Shader>(vertex_shader, fragment_shader);
   mesh->set_shader(shader);

   mesh->set_projection(scale(mat4(1.0), vec3(1, -1, 1)));
}

static void context_reset(void)
{
   mesh.reset();
   texture.reset();
   shader.reset();

   GL::set_function_cb(hw_render.get_proc_address);
   GL::init_symbol_map();

   init_mesh(mesh_path);
}

bool retro_load_game(const struct retro_game_info *info)
{
   enum retro_pixel_format fmt = RETRO_PIXEL_FORMAT_XRGB8888;
   if (!environ_cb(RETRO_ENVIRONMENT_SET_PIXEL_FORMAT, &fmt))
   {
      std::cerr << "XRGB8888 is not supported." << std::endl;
      return false;
   }

#ifdef GLES
   hw_render.context_type = RETRO_HW_CONTEXT_OPENGLES2;
#else
   hw_render.context_type = RETRO_HW_CONTEXT_OPENGL;
#endif

   hw_render.context_reset = context_reset;
   hw_render.depth = true;
   if (!environ_cb(RETRO_ENVIRONMENT_SET_HW_RENDER, &hw_render))
      return false;

   mesh_path = info->path;
   return true;
}

void retro_unload_game(void)
{}

unsigned retro_get_region(void)
{
   return RETRO_REGION_NTSC;
}

bool retro_load_game_special(unsigned, const struct retro_game_info *, size_t)
{
   return false;
}

size_t retro_serialize_size(void)
{
   return 0;
}

bool retro_serialize(void *, size_t)
{
   return false;
}

bool retro_unserialize(const void *, size_t)
{
   return false;
}

void *retro_get_memory_data(unsigned)
{
   return nullptr;
}

size_t retro_get_memory_size(unsigned)
{
   return 0;
}

void retro_reset(void)
{}

void retro_cheat_reset(void)
{}

void retro_cheat_set(unsigned, bool, const char *)
{
}

