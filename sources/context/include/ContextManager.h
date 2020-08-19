/*
MIT License

Copyright (c) 2020 Andrey Vasiliev

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Singleton.h"

struct SDL_Window;
typedef void *SDL_GLContext;

namespace Context {
class CameraMan;
class ShaderResolver;
}

namespace Ogre::RTSS {
class PSSMShadowCameraSetup;
}

namespace Ogre {
class RenderTarget;
class Texture;
class Root;
class SceneManager;
class Camera;
class SceneNode;
class Viewport;
class ShadowCameraSetup;
class RenderWindow;
}

namespace Context {
struct WindowPosition {
  int x, y; //x,y
  int w, h; //width, high
  bool f; //fullscreen
};

struct OpenGlAttribute {
  int major, minor;
};

struct Cursor {
  bool show;
  bool grab;
  bool relative;
};

class ContextManager : public Singleton {
 private:
  void SetupConfigManager();
  void SetupInputs();
  void SetupSDL();
  void SetupOGRE();
  void SetupRTSS();
  void SetupShaderResolver();
  void InitGeneralResources();

 public:
  void Setup();
  void Reset();
  void RestoreScreenSize();
  void SetupGlobal();
  void ResetGlobals();
  void Render();
  void CreateOgreCamera();
  void SetupOgreScenePreconditions();
  void UpdateCursorStatus(Cursor cursor);

  //getters
  Ogre::Root *GetOgreRootPtr() const {
    return ogre_root_;
  }

  Ogre::SceneManager *GetOgreScenePtr() const {
    return ogre_scene_manager_;
  }

  std::shared_ptr<CameraMan> GetCameraMan() const {
    return camera_man_;
  }

  Ogre::Camera *GetOgreCamera() const {
    return ogre_camera_;
  }

  Ogre::SceneNode *GetCameraNode() const {
    return ogre_camera_node_;
  }

  Ogre::Viewport *GetOgreViewport() const {
    return ogre_viewport_;
  }

  const std::string &GetWindowCaption() const {
    return window_caption_;
  }

  const bool IsFullscreen() const {
    return window_position_.f;
  }

  void SetCaption(const std::string &caption) {
    ContextManager::window_caption_ = caption;
  }

  const std::shared_ptr<Ogre::ShadowCameraSetup> &GetOgreShadowCameraSetup() const;

 private:
  //SDL2
  SDL_Window *sdl_window_ = nullptr;
  SDL_GLContext sdl_context_ = nullptr;
  std::string window_caption_ = "My Demo";
  WindowPosition window_position_ = {0, 0, 1024, 768, false};
  WindowPosition actual_monitor_size_ = {0, 0, 0, 0, false};
  OpenGlAttribute opengl_version_ = {3, 3}; //default GL version
  bool opengl_ver_force_ = false;

  //OGRE stuff
  std::string ogre_log_name_ = "Ogre.log";
  Ogre::Root *ogre_root_ = nullptr;
  Ogre::RenderWindow *ogre_render_window_ = nullptr;
  Ogre::SceneManager *ogre_scene_manager_ = nullptr;
  Ogre::SceneNode *ogre_camera_node_ = nullptr;
  Ogre::Viewport *ogre_viewport_= nullptr;
  Ogre::Camera *ogre_camera_ = nullptr;
  std::shared_ptr<CameraMan> camera_man_;
  std::shared_ptr<Ogre::ShadowCameraSetup> ogre_shadow_camera_setup_;

  //RTSS
  std::shared_ptr<ShaderResolver> rtss_material_listener_;
  bool rtss_enable_ = true;
  bool rtss_pssm4_enable_ = true;
  bool rtss_cache_enable_ = false;
  bool rtss_resolver_enable_ = true;
  bool rtss_perpixel_light_enable_ = false;
  bool rtss_perpixel_fog_enable_ = true;
  std::string rtss_cache_dir_ = ".rtss";

  //HLMS
  bool hlms_enable_ = false;
  bool hlms_force_enable_ = false;

  //Global
  std::string media_location_directory_ = "..";
  std::string global_resource_list_file_ = "resources.list";
  bool global_log_enable_ = false;
  bool global_verbose_enable_ = false;
  bool global_sso_enable_ = true;
  bool global_octree_enable_ = true;
  bool global_stbi_enable_ = true;
  bool global_freeimage_enable_ = false;
  bool global_particlefx_enable_ = true;

  bool physics_enable_ = true;
  bool IsPhysicsEnable() const;
  bool IsSoundEnable() const;
  bool sound_enable_ = true;

  bool graphics_debug_show_wireframe_ = false;
  bool graphics_vsync_ = true;
  bool graphics_gamma_enable_ = false;
  int graphics_fsaa_ = 0;
  int graphics_msaa_ = 0;
  std::string graphics_filtration_ = "bilinear";
  int graphics_anisotropy_level_ = 4;
  int graphics_mipmap_count_ = 10;

  //Shadows
  bool graphics_shadows_enable_ = true;
  int graphics_shadows_texture_resolution_ = 1024;
  int graphics_shadows_texture_count_ = 3;
  float graphics_shadows_pssm_0_ = 0.0f;
  float graphics_shadows_pssm_1_ = 0.0f;
  float graphics_shadows_pssm_2_ = 0.0f;
  bool graphics_shadows_split_auto_ = false;
  float graphics_shadows_far_distance_ = 200;
  bool graphics_shadows_self_shadow_ = true;
  float graphics_shadows_split_padding_ = 0.0f;
  bool graphics_shadows_back_faces_ = true;
  std::string graphics_shadows_tecnique_ = "texture";
  std::string graphics_shadows_lighting_ = "additive";
  std::string graphics_shadows_projection_ = "pssm";
  std::string graphics_shadows_material_ = "default";
  std::string graphics_shadows_receiver_material_;
  std::string graphics_shadows_caster_material_ = "PSSM/shadow_caster";
  std::string graphics_shadows_texture_format_ = "DEPTH32";
  bool graphics_shadows_integrated_ = true;

  //Singleton section
 private:
  static ContextManager ContextManagerSingleton;

 public:
  static ContextManager &GetSingleton() {
    return ContextManagerSingleton;
  }

  static ContextManager *GetSingletonPtr() {
    return &ContextManagerSingleton;
  }
}; //class ContextManager
} //namespace Context
