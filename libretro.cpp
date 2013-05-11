#include "libretro.h"
#include "gl.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include <cstring>
#include <string>
#include <iostream>

using namespace GL;
using namespace glm;

static struct retro_hw_render_callback hw_render;
static std::string mesh_path;

static std::vector<std::shared_ptr<Mesh>> meshes;

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
   SYM(glFrontFace)(GL_CW); // When we flip vertically, orientation changes.
   SYM(glEnable)(GL_CULL_FACE);
   SYM(glEnable)(GL_BLEND);

   SYM(glViewport)(0, 0, 640, 480);

   for (auto& mesh : meshes)
      mesh->render();

   SYM(glDisable)(GL_BLEND);

   //auto error = SYM(glGetError)();
   //std::cerr << "GL error: " << error << std::endl;

   video_cb(RETRO_HW_FRAME_BUFFER_VALID, 640, 480, 0);
}

static void init_mesh(const std::string& path)
{
   static const std::string vertex_shader =
      "uniform mat4 uModel;\n"
      "uniform mat4 uMVP;\n"
      "attribute vec4 aVertex;\n"
      "attribute vec3 aNormal;\n"
      "attribute vec2 aTex;\n"
      "varying vec4 vNormal;\n"
      "varying vec2 vTex;\n"
      "varying vec4 vPos;\n"
      "void main() {\n"
      "  gl_Position = uMVP * aVertex;\n"
      "  vTex = vec2(aTex.x, 1.0 - aTex.y);\n"
      "  vPos = uModel * aVertex;\n"
      "  vNormal = uModel * vec4(aNormal, 0.0);\n"
      "}";

   static const std::string fragment_shader =
      "#ifdef GL_ES\n"
      "precision mediump float;\n"
      "#endif\n"
      "varying vec2 vTex;\n"
      "varying vec4 vNormal;\n"
      "varying vec4 vPos;\n"
      "uniform sampler2D sTexture;\n"
      "void main() {\n"
      "  vec4 color = texture2D(sTexture, vTex);\n"
      "  vec3 normal = normalize(vNormal.xyz);\n"
      "  vec3 dist = vPos.xyz - vec3(20.0, 40.0, -30.0);\n"
      "  float directivity = dot(normalize(dist), -normal);\n"
      "  float diffuse = clamp(directivity, 0.0, 1.0) + 0.4;\n"
      "  gl_FragColor = vec4(diffuse * color.rgb, color.a);\n"
      "}";

   auto shader = std::make_shared<Shader>(vertex_shader, fragment_shader);

   meshes = OBJ::load_from_file(path);

   mat4 translation = translate(mat4(1.0), vec3(0, 0, -40));
   mat4 scaler = scale(translation, vec3(15, 15, 15));
   mat4 rotater = rotate(scaler, 0.0f, vec3(0, 1, 0));
   mat4 projection = scale(perspective(45.0f, 640.0f / 480.0f, 1.0f, 100.0f),
         vec3(1, -1, 1));

   for (auto& mesh : meshes)
   {
      mesh->set_model(rotater);
      mesh->set_projection(projection);
      mesh->set_shader(shader);
   }
}

static void context_reset(void)
{
   dead_state = true;
   meshes.clear();
   dead_state = false;

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
{
   dead_state = true;
}

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

