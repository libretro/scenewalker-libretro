#include "libretro.h"
#include "gl.hpp"
#include "mesh.hpp"
#include "object.hpp"
#include "util.hpp"
#include <cstring>
#include <string>
#include <stdint.h>
#include "shared.hpp"

using namespace GL;
using namespace glm;
using namespace std;
using namespace std::tr1;

#define BASE_WIDTH 320
#define BASE_HEIGHT 240
#define MAX_WIDTH (BASE_WIDTH * 6)
#define MAX_HEIGHT (BASE_HEIGHT * 6)
static unsigned width = BASE_WIDTH;
static unsigned height = BASE_HEIGHT;

static struct retro_hw_render_callback hw_render;
static string mesh_path;

static vector<std::tr1::shared_ptr<Mesh> > meshes;

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
   memset(info, 0, sizeof(*info));
   info->library_name     = "ModelViewer";
   info->library_version  = "v1";
   info->need_fullpath    = true;
   info->valid_extensions = "obj";
}

void retro_get_system_av_info(struct retro_system_av_info *info)
{
   memset(info, 0, sizeof(*info));
   info->timing.fps = 60.0;
   info->timing.sample_rate = 30000.0;

   info->geometry.base_width  = BASE_WIDTH;
   info->geometry.base_height = BASE_HEIGHT;
   info->geometry.max_width   = MAX_WIDTH;
   info->geometry.max_height  = MAX_HEIGHT;
}

static retro_video_refresh_t video_cb;
static retro_audio_sample_t audio_cb;
static retro_audio_sample_batch_t audio_batch_cb;
static retro_environment_t environ_cb;
static retro_input_poll_t input_poll_cb;
static retro_input_state_t input_state_cb;

void retro_stderr(const char *str)
{
#ifdef _WIN32
   OutputDebugStringA(str);
#else
   fprintf(stderr, str);
#endif
}

void retro_set_environment(retro_environment_t cb)
{
   environ_cb = cb;

   retro_variable variables[] = {
      { "modelviewer_resolution",
         "Internal resolution; 320x240|640x480|960x720|1280x960|1600x1200|1920x1440" },
      { NULL, NULL },
   };

   cb(RETRO_ENVIRONMENT_SET_VARIABLES, variables);
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

static void handle_input()
{
   static float model_rotate_y;
   static float model_rotate_x;
   static float model_scale = 1.0f;

   input_poll_cb();

   int analog_x = input_state_cb(0, RETRO_DEVICE_ANALOG,
         RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_X);

   int analog_y = input_state_cb(0, RETRO_DEVICE_ANALOG,
         RETRO_DEVICE_INDEX_ANALOG_LEFT, RETRO_DEVICE_ID_ANALOG_Y);

   int analog_ry = input_state_cb(0, RETRO_DEVICE_ANALOG,
         RETRO_DEVICE_INDEX_ANALOG_RIGHT, RETRO_DEVICE_ID_ANALOG_Y);

   if (abs(analog_x) < 10000)
      analog_x = 0;
   if (abs(analog_y) < 10000)
      analog_y = 0;

   if (abs(analog_ry) < 10000)
      analog_ry = 0;

   model_scale *= 1.0f - analog_ry * 0.000001f;
   model_scale = clamp(model_scale, 0.1f, 100.0f);
   model_rotate_x += analog_y * 0.0001f;
   model_rotate_x = clamp(model_rotate_x, -80.0f, 80.0f);
   model_rotate_y += analog_x * 0.00015f;

   mat4 translation = translate(mat4(1.0), vec3(0, 0, -40));
   mat4 scaler = scale(mat4(1.0), vec3(model_scale, model_scale, model_scale));
   mat4 rotate_x = rotate(mat4(1.0), model_rotate_x, vec3(1, 0, 0));
   mat4 rotate_y = rotate(mat4(1.0), model_rotate_y, vec3(0, 1, 0));

   mat4 model = translation * scaler * rotate_x * rotate_y;

   for (unsigned i = 0; i < meshes.size(); i++)
      meshes[i]->set_model(model);
}

static void update_variables()
{
   retro_variable var;
   var.key = "modelviewer_resolution";
   var.value = NULL;

   if (!environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE, &var) || !var.value)
      return;

   std::vector<std::string> list = String::split(var.value, "x");
   if (list.size() != 2)
      return;

   width = String::stoi(list[0]);
   height = String::stoi(list[1]);
}

void retro_run(void)
{
   handle_input();

   bool updated = false;
   if (environ_cb(RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE, &updated) && updated)
      update_variables();

   SYM(glBindFramebuffer)(GL_FRAMEBUFFER, hw_render.get_current_framebuffer());
   SYM(glClearColor)(0.2f, 0.2f, 0.2f, 1.0f);
   SYM(glClear)(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
   SYM(glEnable)(GL_DEPTH_TEST);
   SYM(glFrontFace)(GL_CW); // When we flip vertically, orientation changes.
   SYM(glEnable)(GL_CULL_FACE);
   SYM(glEnable)(GL_BLEND);

   SYM(glViewport)(0, 0, width, height);

   for (unsigned i = 0; i < meshes.size(); i++)
      meshes[i]->render();

   SYM(glDisable)(GL_BLEND);
   SYM(glDisable)(GL_DEPTH_TEST);
   SYM(glDisable)(GL_CULL_FACE);
   SYM(glBindFramebuffer)(GL_FRAMEBUFFER, 0);
   video_cb(RETRO_HW_FRAME_BUFFER_VALID, width, height, 0);
}

static void init_mesh(const string& path)
{
   static const string vertex_shader =
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

   static const string fragment_shader =
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

   std::tr1::shared_ptr<Shader> shader(new Shader(vertex_shader, fragment_shader));
   meshes = OBJ::load_from_file(path);

   mat4 projection = scale(mat4(1.0), vec3(1, -1, 1)) * perspective(45.0f, 640.0f / 480.0f, 1.0f, 100.0f);

   for (unsigned i = 0; i < meshes.size(); i++)
   {
      meshes[i]->set_projection(projection);
      meshes[i]->set_shader(shader);
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
      retro_stderr("XRGB8888 is not supported.");
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
   update_variables();
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
   return NULL;
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

