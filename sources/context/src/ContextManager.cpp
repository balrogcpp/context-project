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

#include "pcheader.hpp"

#include "ContextManager.hpp"
#include "ConfigManager.hpp"
#include "Exception.hpp"
#include "CameraMan.hpp"
#include "ShaderResolver.hpp"
#include "PhysicsManager.hpp"
#include "AppStateManager.hpp"
#include "DotSceneLoaderB.hpp"
#include "StaticForestManager.hpp"
#include "PagedForestManager.hpp"
#include "SoundManager.hpp"
#include "CompositorManager.hpp"
#include "CeguiOverlayManager.hpp"
#include "PSSMShadowCameraSetupB.hpp"
#include "CubeMapCamera.hpp"
#include "ReflectionCamera.hpp"

extern "C" {
#include <SDL2/SDL_syswm.h>
}

namespace Context {
ContextManager ContextManager::ContextManagerSingleton;

//----------------------------------------------------------------------------------------------------------------------
void ContextManager::SetupConfigManager() {
  ConfigManager::Assign(window_position_.f, "window_fullscreen");
  ConfigManager::Assign(window_position_.h, "window_high");
  ConfigManager::Assign(window_position_.w, "window_width");
  ConfigManager::Assign(window_caption_, "window_caption");

  ConfigManager::Assign(opengl_version_.major, "opengl_ver_major");
  ConfigManager::Assign(opengl_version_.minor, "opengl_ver_minor");
  ConfigManager::Assign(opengl_ver_force_, "opengl_ver_force");

  ConfigManager::Assign(rtss_enable_, "rtss_enable");
  ConfigManager::Assign(rtss_pssm4_enable_, "rtss_pssm4_enable");
  ConfigManager::Assign(rtss_resolver_enable_, "rtss_resolver_enable");
  ConfigManager::Assign(rtss_cache_enable_, "rtss_cache_enable");
  ConfigManager::Assign(rtss_perpixel_light_enable_, "rtss_perpixel_light_enable");
  ConfigManager::Assign(rtss_perpixel_fog_enable_, "rtss_perpixel_fog_enable");
  ConfigManager::Assign(rtss_cache_dir_, "rtss_cache_dir");

  ConfigManager::Assign(hlms_enable_, "hlms_enable");
  ConfigManager::Assign(hlms_force_enable_, "hlms_force_enable");

  ConfigManager::Assign(global_octree_enable_, "global_octree_enable");
  ConfigManager::Assign(global_verbose_enable_, "global_verbose_enable");
  ConfigManager::Assign(global_log_enable_, "global_log_enable");
  ConfigManager::Assign(ogre_log_name_, "global_log_name");
  ConfigManager::Assign(global_stbi_enable_, "global_stbi_enable");
  ConfigManager::Assign(global_freeimage_enable_, "global_freeimage_enable");
  ConfigManager::Assign(global_particlefx_enable_, "global_particlefx_enable");
  ConfigManager::Assign(physics_enable_, "physics_enable");
  ConfigManager::Assign(sound_enable_, "sound_enable");
#ifdef DEBUG
  ConfigManager::Assign(media_location_directory_, "media_location_directory");
#endif
  ConfigManager::Assign(global_resource_list_file_, "global_resource_list");

  ConfigManager::Assign(graphics_debug_show_wireframe_, "graphics_debug_show_wireframe");
  ConfigManager::Assign(graphics_vsync_, "graphics_vsync");
  ConfigManager::Assign(graphics_gamma_enable_, "graphics_gamma");
  ConfigManager::Assign(graphics_fsaa_, "graphics_fsaa");
  ConfigManager::Assign(graphics_msaa_, "graphics_msaa");
  ConfigManager::Assign(graphics_filtration_, "graphics_filtration");
  ConfigManager::Assign(graphics_anisotropy_level_, "graphics_anisotropy_level");
  ConfigManager::Assign(graphics_mipmap_count_, "graphics_mipmap_count");

  ConfigManager::Assign(graphics_shadows_enable_, "graphics_shadows_enable");
  ConfigManager::Assign(graphics_shadows_pssm_enable_, "graphics_shadows_pssm_enable");
  ConfigManager::Assign(graphics_shadows_depth_enable_, "graphics_shadows_depth_enable");
  ConfigManager::Assign(graphics_shadows_pssm_0_, "graphics_shadows_pssm_0");
  ConfigManager::Assign(graphics_shadows_pssm_1_, "graphics_shadows_pssm_1");
  ConfigManager::Assign(graphics_shadows_pssm_2_, "graphics_shadows_pssm_2");
  ConfigManager::Assign(graphics_shadows_split_auto_, "graphics_shadows_split_auto");
  ConfigManager::Assign(graphics_shadows_pssm_0_resolution_, "graphics_shadows_pssm_0_resolution");
  ConfigManager::Assign(graphics_shadows_pssm_1_resolution_, "graphics_shadows_pssm_1_resolution");
  ConfigManager::Assign(graphics_shadows_pssm_2_resolution_, "graphics_shadows_pssm_2_resolution");
  ConfigManager::Assign(graphics_shadows_texture_resolution_, "graphics_shadows_texture_resolution");
  ConfigManager::Assign(graphics_shadows_split_padding_, "graphics_shadows_split_padding");
  ConfigManager::Assign(graphics_shadows_texture_count_, "graphics_shadows_texture_count");
  ConfigManager::Assign(graphics_shadows_far_distance_, "graphics_shadows_far_distance");
  ConfigManager::Assign(graphics_shadows_self_shadow_, "graphics_shadows_self_shadow");
  ConfigManager::Assign(graphics_shadows_back_faces_, "graphics_shadows_back_faces");
  ConfigManager::Assign(graphics_shadows_caster_material_, "graphics_shadows_caster_material");
  ConfigManager::Assign(graphics_shadows_receiver_material_, "graphics_shadows_receiver_material");

  ConfigManager::Assign(graphics_shadows_tecnique_, "graphics_shadows_tecnique");
  ConfigManager::Assign(graphics_shadows_lighting_, "graphics_shadows_lighting");
  ConfigManager::Assign(graphics_shadows_projection_, "graphics_shadows_projection");
  ConfigManager::Assign(graphics_shadows_material_, "graphics_shadows_material");
  ConfigManager::Assign(graphics_shadows_integrated_, "graphics_shadows_integrated");
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::SetupPath() {
#ifndef DEBUG
  media_location_directory_ = "./";
#else
  media_location_directory_ = "../../../";
#endif
}
//----------------------------------------------------------------------------------------------------------------------
static bool StringSanityCheck(const std::string &str) {
  if (str[0] == '#') {
    return true;
  }

  for (const auto &it : str) {
    if (std::isalpha(it) || std::isdigit(it) || it == '.' || it == ',' || it == ';' || it == '_' || it == '-'
        || it == '/') {
      //it's Ok
    } else {
      return false;
    }
  }

  return true;
}
//----------------------------------------------------------------------------------------------------------------------
static std::string &LeftTrim(std::string &s) {
  auto it = std::find_if(s.begin(), s.end(), [](char c) {
    return !std::isspace<char>(c, std::locale::classic());
  });
  s.erase(s.begin(), it);
  return s;
}
//----------------------------------------------------------------------------------------------------------------------
static std::string &RightTrim(std::string &s) {
  auto it = std::find_if(s.rbegin(), s.rend(), [](char c) { return !std::isspace<char>(c, std::locale::classic()); });
  s.erase(it.base(), s.end());
  return s;
}
//----------------------------------------------------------------------------------------------------------------------
static std::string &TrimString(std::string &s) {
  return LeftTrim(RightTrim(s));
}
//----------------------------------------------------------------------------------------------------------------------
static void PrintPathList(const std::vector<std::tuple<std::string, std::string, std::string>> &path_list) {
  std::cout << "Path list:\n";

  for (const auto &it : path_list) {
    std::cout << "Path: " << std::get<0>(it) << "; Type: " << std::get<1>(it) << "; Group: " << std::get<2>(it)
              << ";\n";
  }

  std::cout << '\n';
}
//----------------------------------------------------------------------------------------------------------------------
static void PrintStringList(const std::vector<std::string> &string_list) {
  std::cout << "Path list:\n";

  for (const auto &it : string_list) {
    std::cout << "String : " << it << ";\n";
  }

  std::cout << '\n';
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::InitGeneralResources() {
  namespace fs = std::filesystem;
  const std::string default_group_name = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;

  std::vector<std::string> file_list;
  std::vector<std::string> path_list;
  std::vector<std::tuple<std::string, std::string, std::string>> resource_list;
  auto &ogre_resource_manager = Ogre::ResourceGroupManager::getSingleton();

  resource_list.push_back({media_location_directory_ + "programs", "FileSystem", default_group_name});
  resource_list.push_back({media_location_directory_ + "scenes", "FileSystem", default_group_name});

#ifdef DEBUG
  resource_list.push_back({media_location_directory_ + "gui/animations", "FileSystem", "Animation"});
  resource_list.push_back({media_location_directory_ + "gui/fonts", "FileSystem", "Fonts"});
  resource_list.push_back({media_location_directory_ + "gui/imagesets", "FileSystem", "Imagesets"});
  resource_list.push_back({media_location_directory_ + "gui/layouts", "FileSystem", "Layouts"});
  resource_list.push_back({media_location_directory_ + "gui/looknfeel", "FileSystem", "LookNFeel"});
  resource_list.push_back({media_location_directory_ + "gui/lua_scripts", "FileSystem", "LuaScripts"});
  resource_list.push_back({media_location_directory_ + "gui/schemes", "FileSystem", "Schemes"});
  resource_list.push_back({media_location_directory_ + "gui/xml_schemas", "FileSystem", "XmlSchemas"});
#else
  ogre_resource_manager.addResourceLocation(media_location_directory_ + "gui/animations.zip", "Zip", "Animation");
  ogre_resource_manager.addResourceLocation(media_location_directory_ + "gui/fonts.zip", "Zip", "Fonts");
  ogre_resource_manager.addResourceLocation(media_location_directory_ + "gui/imagesets.zip", "Zip", "Imagesets");
  ogre_resource_manager.addResourceLocation(media_location_directory_ + "gui/layouts.zip", "Zip", "Layouts");
  ogre_resource_manager.addResourceLocation(media_location_directory_ + "gui/looknfeel.zip", "Zip", "LookNFeel");
  ogre_resource_manager.addResourceLocation(media_location_directory_ + "gui/lua_scripts.zip", "Zip", "LuaScripts");
  ogre_resource_manager.addResourceLocation(media_location_directory_ + "gui/schemes.zip", "Zip", "Schemes");
  ogre_resource_manager.addResourceLocation(media_location_directory_ + "gui/xml_schemas.zip", "Zip", "XmlSchemas");
#endif

  if (rtss_cache_enable_) {
    if (!std::filesystem::exists(rtss_cache_dir_)) {
      std::filesystem::create_directory(rtss_cache_dir_);
      resource_list.push_back({rtss_cache_dir_, "FileSystem", default_group_name});
    }

  }

  if (!global_resource_list_file_.empty()) {
    std::fstream list_file;
    list_file.open(global_resource_list_file_);

    std::string line;
    std::string path;
    std::string type;
    std::string group;

    if (list_file.is_open()) {
      while (getline(list_file, line)) {
        line = TrimString(line);

        if (!StringSanityCheck(line)) {
          throw Exception(std::string("Sanity check of file ") + global_resource_list_file_ + " is failed. Aborting.");
        }

        if (line[0] == '#') {
          continue;
        }

        std::stringstream ss(line);
        getline(ss, path, ',');
        getline(ss, type, ',');
        getline(ss, group, ';');
        resource_list.push_back({path, type, group});
      }

      list_file.close();
    }
  }

  if (global_verbose_enable_) {
    PrintPathList(resource_list);
  }

  const std::vector<std::string> extensions_list =
      {".glsl", ".glslt", ".hlsl", ".hlslt", ".gles", ".cg", ".vert", ".frag", ".material", ".compositor", ".particle",
       ".fx", ".program", ".dds", ".bmp", ".png", ".tga", ".jpg",
       ".jpeg", ".mesh", ".xml", ".scene", ".json", ".wav", ".ogg", ".mp3", ".flac"};

  for (const auto &it : resource_list) {
    ogre_resource_manager.addResourceLocation(std::get<0>(it), std::get<1>(it), std::get<2>(it));
    if (std::find(std::begin(path_list), std::end(path_list), std::get<0>(it)) == std::end(path_list)) {
      path_list.push_back(std::get<0>(it));
    } else {
      throw Exception("Path " + std::get<0>(it) + " already registered. Aborting.");
    }

    for (auto jt = fs::recursive_directory_iterator(std::get<0>(it)); jt != fs::recursive_directory_iterator(); ++jt) {
      const auto file_path = jt->path().string();
      const auto file_name = jt->path().filename().string();

      if (jt->is_directory()) {
        if (global_verbose_enable_) {
          std::cout << "Parsing directory:  " << file_path << '\n';
        }
        if (std::find(std::begin(path_list), std::end(path_list), file_name) == std::end(path_list)) {
          path_list.push_back(file_name);
        } else {
          throw Exception("Path " + file_name + " already registered. Aborting.");
        }

        ogre_resource_manager.addResourceLocation(file_path, "FileSystem", std::get<2>(it));

      } else if (jt->is_regular_file()) {
        if (global_verbose_enable_) {
          std::cout << "Parsing file:  " << file_path << '\n';
        }
        if (fs::path(file_path).extension() == ".zip") {
          if (std::find(std::begin(file_list), std::end(file_list), file_name) == std::end(file_list)) {
            if (std::find(std::begin(extensions_list), std::end(extensions_list), fs::path(file_name).extension())
                != std::end(extensions_list)) {
              file_list.push_back(file_name);
            }
          } else {
            throw Exception("File " + file_name + " already exists. Aborting.");
          }

          ogre_resource_manager.addResourceLocation(file_path, "Zip", std::get<2>(it));

        }
      }
    }
  }

  // load resources
  Ogre::ResourceGroupManager::getSingletonPtr()->initialiseAllResourceGroups();
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::CreateOgreCamera() {
  ogre_camera_node_ = ogre_scene_manager_->getRootSceneNode()->createChildSceneNode();

  if (!ogre_scene_manager_->hasCamera("Camera")) {
    ogre_camera_ = (ogre_scene_manager_->createCamera("Camera"));
    auto *renderTarget = ogre_root_->getRenderTarget(this->window_caption_);
    renderTarget->removeViewport(0);
    ogre_viewport_ = renderTarget->addViewport(ogre_camera_);
    ogre_viewport_->setBackgroundColour(Ogre::ColourValue::Black);
    ogre_viewport_->setBackgroundColour(Ogre::ColourValue::Black);
    ogre_viewport_->setDimensions(0, 0, 1, 1);
  }
  if (!camera_man_) {
    camera_man_ = std::make_shared<CameraMan>();
  }

  camera_man_->RegCamera(ogre_camera_node_);
  camera_man_->SetStyle(CameraStyle::MANUAL);

  if (ogre_camera_) {
    ogre_camera_->setNearClipDistance(0.01f);
    ogre_camera_->setFarClipDistance(10000.0f);
  }

  ogre_camera_->setAspectRatio(
      static_cast<float>(ogre_viewport_->getActualWidth()) / static_cast<float>(ogre_viewport_->getActualHeight()));

  ogre_scene_manager_->setSkyBoxEnabled(false);
  ogre_scene_manager_->setSkyDomeEnabled(false);
  ogre_scene_manager_->setAmbientLight(Ogre::ColourValue::Black);
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::SetupOgreScenePreconditions() {
  CreateOgreCamera();

  // Texture filtering
  if (graphics_filtration_ == "anisotropic") {
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_ANISOTROPIC);
    Ogre::MaterialManager::MaterialManager::getSingleton().setDefaultAnisotropy(graphics_anisotropy_level_);
  } else if (graphics_filtration_ == "bilinear") {
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_BILINEAR);
  } else if (graphics_filtration_ == "trilinear") {
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_TRILINEAR);
  } else if (graphics_filtration_ == "none") {
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_NONE);
  } else {
    Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(Ogre::TFO_BILINEAR);
  }

  if (graphics_mipmap_count_ > 0) {
    if (graphics_mipmap_count_ != 999) {
      Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(graphics_mipmap_count_);
    } else {
      Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(Ogre::MIP_UNLIMITED);
    }
  }

  //Global shadows
  if (graphics_shadows_enable_) {
    unsigned shadow_technique = Ogre::SHADOWTYPE_NONE;

    if (graphics_shadows_tecnique_ == "stencil") {
      shadow_technique = Ogre::SHADOWDETAILTYPE_STENCIL;
    } else if (graphics_shadows_tecnique_ == "texture") {
      shadow_technique = Ogre::SHADOWDETAILTYPE_TEXTURE;

      if (graphics_shadows_integrated_) {
        shadow_technique |= Ogre::SHADOWDETAILTYPE_INTEGRATED;
      }
    }

    if (graphics_shadows_lighting_ == "modulative") {
      shadow_technique |= Ogre::SHADOWDETAILTYPE_MODULATIVE;
    } else if (graphics_shadows_lighting_ == "additive") {
      shadow_technique |= Ogre::SHADOWDETAILTYPE_ADDITIVE;
    }

    ogre_scene_manager_->setShadowTechnique(static_cast<Ogre::ShadowTechnique>(shadow_technique));

    if (graphics_shadows_material_ == "default") {
//      auto texture_type = Ogre::PF_FLOAT16_R;
//      auto texture_type = Ogre::PF_FLOAT32_R;
      auto texture_type = Ogre::PF_FLOAT32_GR;

      ogre_scene_manager_->setShadowTextureSettings(graphics_shadows_texture_resolution_,
                                                    graphics_shadows_texture_count_,
                                                    texture_type);

      ogre_scene_manager_->setShadowTextureConfig(0,
                                                  graphics_shadows_pssm_0_resolution_,
                                                  graphics_shadows_pssm_0_resolution_,
                                                  texture_type);
      ogre_scene_manager_->setShadowTextureConfig(1,
                                                  graphics_shadows_pssm_1_resolution_,
                                                  graphics_shadows_pssm_1_resolution_,
                                                  texture_type);
      ogre_scene_manager_->setShadowTextureConfig(2,
                                                  graphics_shadows_pssm_2_resolution_,
                                                  graphics_shadows_pssm_2_resolution_,
                                                  texture_type);

      ogre_scene_manager_->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
      ogre_scene_manager_->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 3);
      ogre_scene_manager_->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 6);
      ogre_scene_manager_->setShadowTextureSelfShadow(graphics_shadows_self_shadow_);
      ogre_scene_manager_->setShadowCasterRenderBackFaces(graphics_shadows_back_faces_);
      ogre_scene_manager_->setShadowFarDistance(graphics_shadows_far_distance_);

      if (!graphics_shadows_caster_material_.empty()) {
        auto passCaterMaterial = Ogre::MaterialManager::getSingleton().getByName(graphics_shadows_caster_material_);
        ogre_scene_manager_->setShadowTextureCasterMaterial(passCaterMaterial);
      }

      if (!graphics_shadows_receiver_material_.empty()) {
        auto
            passReceiverMaterial = Ogre::MaterialManager::getSingleton().getByName(graphics_shadows_receiver_material_);
        ogre_scene_manager_->setShadowTextureReceiverMaterial(passReceiverMaterial);
      }

      auto pssm_setup = std::make_shared<Ogre::PSSMShadowCameraSetup>();

      pssm_setup->setUseAggressiveFocusRegion(false);

      if (!ogre_shadow_camera_setup_) {
        if (graphics_shadows_split_auto_) {
          pssm_setup->calculateSplitPoints(3, ogre_camera_->getNearClipDistance(),
                                           ogre_scene_manager_->getShadowFarDistance());
        } else {
          Ogre::PSSMShadowCameraSetup::SplitPointList split_points =
              {ogre_camera_->getNearClipDistance(), 10.0, 20.0, ogre_scene_manager_->getShadowFarDistance()};
          pssm_setup->setSplitPoints(split_points);
        }

        if (graphics_shadows_split_padding_ == 0) {
          pssm_setup->setSplitPadding(1.0);
        } else {
          pssm_setup->setSplitPadding(graphics_shadows_split_padding_);
        }

        pssm_setup->setOptimalAdjustFactor(0, graphics_shadows_pssm_0_);
        pssm_setup->setOptimalAdjustFactor(1, graphics_shadows_pssm_1_);
        pssm_setup->setOptimalAdjustFactor(2, graphics_shadows_pssm_2_);

#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
        if (rtss_enable_ && rtss_pssm4_enable_) {
          // Make this viewport work with shader generator scheme.
          Ogre::RTShader::ShaderGenerator &rtShaderGen = Ogre::RTShader::ShaderGenerator::getSingleton();

          auto subRenderState = rtShaderGen.createSubRenderState<Ogre::RTShader::IntegratedPSSM3>();
          Ogre::RTShader::RenderState
              *schemRenderState = rtShaderGen.getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

          subRenderState->setSplitPoints(pssm_setup->getSplitPoints());
          schemRenderState->addTemplateSubRenderState(subRenderState);
        }
#endif
        ogre_shadow_camera_setup_ = pssm_setup;
      }
    }

    if (graphics_shadows_projection_ == "uniform") {
      ogre_shadow_camera_setup_ = Ogre::DefaultShadowCameraSetup::create();
    } else if (graphics_shadows_projection_ == "uniform_focused") {
      ogre_shadow_camera_setup_ = Ogre::FocusedShadowCameraSetup::create();
    } else if (graphics_shadows_projection_ == "lspsm") {
      ogre_shadow_camera_setup_ = Ogre::LiSPSMShadowCameraSetup::create();
    } else if (graphics_shadows_projection_ == "pssm") {

    } else {
      ogre_shadow_camera_setup_ = Ogre::DefaultShadowCameraSetup::create();
    }

    if (ogre_shadow_camera_setup_) {
      ogre_scene_manager_->setShadowCameraSetup(ogre_shadow_camera_setup_);
    }

  } else {
    ogre_scene_manager_->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  }

#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  if (rtss_enable_ && rtss_perpixel_fog_enable_) {
    // Make this viewport work with shader generator scheme.
    Ogre::RTShader::ShaderGenerator &rtShaderGen = Ogre::RTShader::ShaderGenerator::getSingleton();

    Ogre::RTShader::RenderState
        *schemRenderState = rtShaderGen.getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

    auto subRenderState2 = rtShaderGen.createSubRenderState<Ogre::RTShader::FFPFog>();
    subRenderState2->setCalcMode(Ogre::RTShader::FFPFog::CM_PER_PIXEL);
    schemRenderState->addTemplateSubRenderState(subRenderState2);

    rtShaderGen.invalidateScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  }
#endif

  if (graphics_debug_show_wireframe_) {
    ContextManager::GetSingleton().GetOgreCamera()->setPolygonMode(Ogre::PM_WIREFRAME);
  } else {
    ContextManager::GetSingleton().GetOgreCamera()->setPolygonMode(Ogre::PM_SOLID);
  }

}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::SetupInputs() {
  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) < 0)
    throw Exception("Failed to init SDL2");

  if (SDL_NumJoysticks() != 0) {
    if (global_verbose_enable_) {
      std::cout << SDL_NumJoysticks() << " joystick(s) detected\n";
    }

    for (int i = 0; i < SDL_NumJoysticks(); i++) {
      if (SDL_IsGameController(i)) {
        SDL_GameController *controller = nullptr;

        controller = SDL_GameControllerOpen(i);

        break;
      }
    }

  }

}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::SetupSDL() {
  SDL_DisplayMode DM;
  SDL_GetCurrentDisplayMode(0, &DM);

  actual_monitor_size_.w = static_cast<int>(DM.w);
  actual_monitor_size_.h = static_cast<int>(DM.h);
  actual_monitor_size_.f = window_position_.f;

  bool invalid = (window_position_.w * window_position_.h) == 0;

  if (invalid)
    window_position_.w = actual_monitor_size_.w;
  if (invalid)
    window_position_.h = actual_monitor_size_.h;
  if (invalid)
    window_position_.f = true;

  bool factual_fullscreen = (window_position_.w == actual_monitor_size_.w && window_position_.h == actual_monitor_size_.h);

  uint32_t flags = 0x0;

  flags |= SDL_WINDOW_ALLOW_HIGHDPI;

  if (factual_fullscreen) {
    flags |= SDL_WINDOW_BORDERLESS;
  }

  if (window_position_.f) {
    flags |= SDL_WINDOW_ALWAYS_ON_TOP;
    flags |= SDL_WINDOW_INPUT_GRABBED;
    flags |= SDL_WINDOW_MOUSE_FOCUS;
  }

  sdl_window_ = SDL_CreateWindow(window_caption_.c_str(),
                                 SDL_WINDOWPOS_CENTERED,
                                 SDL_WINDOWPOS_CENTERED,
                                 window_position_.w,
                                 window_position_.h,
                                 flags);

  if (sdl_window_) {
    SDL_SetRelativeMouseMode(SDL_TRUE);
  } else {
    throw Exception("Failed to Create SDL_Window");
  }

  if (opengl_ver_force_) {
    constexpr std::array<std::pair<int, int>, 10> ver = {std::make_pair(4, 5),
                                                         std::make_pair(4, 4),
                                                         std::make_pair(4, 3),
                                                         std::make_pair(4, 2),
                                                         std::make_pair(4, 1),
                                                         std::make_pair(4, 0),
                                                         std::make_pair(3, 3),
                                                         std::make_pair(3, 2),
                                                         std::make_pair(3, 1),
                                                         std::make_pair(3, 0)
    };

    if (opengl_ver_force_) {
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, opengl_version_.major);
      SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, opengl_version_.minor);

      sdl_context_ = SDL_GL_CreateContext(sdl_window_);

      if (!sdl_context_) {
        for (const auto &it : ver) {
          SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
          SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, it.first);
          SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, it.second);

          sdl_context_ = SDL_GL_CreateContext(sdl_window_);

          if (!sdl_context_) {
            continue;
          } else {
            opengl_version_.major = it.first;
            opengl_version_.minor = it.second;
            if (global_verbose_enable_) {
              std::cout << "Context initialized with OpenGL ";
              std::cout << it.first;
              std::cout << ".";
              std::cout << it.second;
              std::cout << '\n';
            }
            break;
          }
        }
      }
    } else {
      for (auto &it : ver) {
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, it.first);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, it.second);

        sdl_context_ = SDL_GL_CreateContext(sdl_window_);

        if (!sdl_context_) {
          continue;
        } else {
          opengl_version_.major = it.first;
          opengl_version_.minor = it.second;
          if (global_verbose_enable_) {
            std::cout << "Context initialized with OpenGL ";
            std::cout << it.first;
            std::cout << ".";
            std::cout << it.second;
            std::cout << '\n';
          }
          break;
        }
      }
    }

    if (!sdl_context_) {
      throw Exception("Failed to Create SDL_GL_Context");
    }

  }
}
//
//
//
/////////////////////////////////////////////////////////////////////////////////////
void ContextManager::SetupOGRE() {
  if (global_log_enable_) {
    ogre_root_ = new Ogre::Root("", "", ogre_log_name_);
  } else {
    ogre_root_ = new Ogre::Root("", "", "");
  }
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  auto *mGL3PlusRenderSystem = new Ogre::GL3PlusRenderSystem();
  Ogre::Root::getSingleton().setRenderSystem(mGL3PlusRenderSystem);
#else
#ifdef OGRE_BUILD_RENDERSYSTEM_GL
  auto *mGLRenderSystem = new Ogre::GLRenderSystem();
  Ogre::Root::getSingleton().setRenderSystem(mGLRenderSystem);
#endif
#endif //OGRE_BUILD_RENDERSYSTEM_GL3PLUS
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  if (global_octree_enable_) {
    auto *mOctreePlugin = new Ogre::OctreeSceneManagerFactory();
    Ogre::Root::getSingleton().addSceneManagerFactory(mOctreePlugin);
  }
#endif // OGRE_BUILD_PLUGIN_OCTREE
#ifdef OGRE_BUILD_PLUGIN_PFX
  if (global_particlefx_enable_) {
    auto *mParticlePlugin = new Ogre::ParticleFXPlugin();
    Ogre::Root::getSingleton().installPlugin(mParticlePlugin);
  }
#endif // OGRE_BUILD_PLUGIN_PFX
#ifdef OGRE_BUILD_PLUGIN_STBI
  if (global_stbi_enable_) {
    auto *mSTBIImageCodec = new Ogre::STBIPlugin();
    Ogre::Root::getSingleton().installPlugin(mSTBIImageCodec);
  }
#endif // OGRE_BUILD_PLUGIN_STBI
#ifdef OGRE_BUILD_PLUGIN_FREEIMAGE
  if (global_freeimage_enable_) {
    auto *mFreeImageCodec = new Ogre::FreeImagePlugin();
    Ogre::Root::getSingleton().installPlugin(mFreeImageCodec);
  }
#endif
  ogre_root_->initialise(false);
  Ogre::NameValuePairList params;

  SDL_SysWMinfo info;
  SDL_VERSION(&info.version);
  SDL_GetWindowWMInfo(sdl_window_, &info);
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  if (!reinterpret_cast<size_t>(info.info.win.window)) {
    throw Exception("Cast from info.info.win.window to size_t failed");
  }

  params["externalWindowHandle"] = std::to_string(reinterpret_cast<size_t>(info.info.win.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  if (!reinterpret_cast<unsigned long>(info.info.x11.window)) {
    throw Exception("Cast from info.info.x11.window to size_t failed");
  }

  params["externalWindowHandle"] = std::to_string(reinterpret_cast<unsigned long>(info.info.x11.window));
#endif

  const char true_str[] = "true";
  const char false_str[] = "false";

  params["vsync"] = graphics_vsync_ ? true_str : false_str;
  params["gamma"] = graphics_gamma_enable_ ? true_str : false_str;
  params["FSAA"] = std::to_string(graphics_fsaa_);
  params["MSAA"] = std::to_string(graphics_msaa_);

  ogre_render_window_ = ogre_root_->createRenderWindow(window_caption_, static_cast<unsigned>(window_position_.w), \
                       static_cast<unsigned>(window_position_.h), false, &params);

#ifdef OGRE_BUILD_PLUGIN_OCTREE
  if (global_octree_enable_) {
    ogre_scene_manager_ = ogre_root_->createSceneManager("OctreeSceneManager");
  } else {
    ogre_scene_manager_ = ogre_root_->createSceneManager();
  }
#else
  scene_mgr_ = ogre_root->createSceneManager();
#endif

  if (global_verbose_enable_) {
    std::cout << "Scene Managers supported:\n";
    std::cout << "--------------------------------------------\n";

    for (auto &it : Ogre::SceneManagerEnumerator::getSingleton().getMetaDataIterator()) {
      std::string worldGeometrySupported = it->worldGeometrySupported ? "True" : "False";
      std::cout << it->typeName << " " << worldGeometrySupported << '\n';
    }
  }

  ogre_camera_ = (ogre_scene_manager_->createCamera("Camera"));
  auto *renderTarget = ogre_root_->getRenderTarget(this->window_caption_);

  ogre_viewport_ = renderTarget->addViewport(ogre_camera_);
  ogre_viewport_->setBackgroundColour(Ogre::ColourValue::Black);

}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::SetupRTSS() {
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  if (rtss_enable_) {
    if (!Ogre::RTShader::ShaderGenerator::initialize()) {
      throw Exception("RTTS System failed to initialize");
    }

    ogre_viewport_->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
  }
#endif
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::SetupShaderResolver() {
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  if (rtss_enable_) {
    auto *mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();
    mShaderGenerator->addSceneManager(ogre_scene_manager_);
    ogre_viewport_->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

    if (rtss_resolver_enable_) {
      // Create and register the material manager listener if it doesn't exist yet.
      if (!rtss_material_listener_) {
        rtss_material_listener_ = std::make_shared<ShaderResolver>(mShaderGenerator);
        Ogre::MaterialManager::getSingleton().addListener(rtss_material_listener_.get());
      }
    }

    // Add per pixel lighting sub Render state to the global scheme Render state.
    // It will override the default FFP lighting sub Render state.
    if (rtss_perpixel_light_enable_) {
      // Grab the scheme Render state.
      Ogre::RTShader::RenderState *schemRenderState =
          mShaderGenerator->getRenderState(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);

      Ogre::RTShader::SubRenderState *perPixelLightModel =
          mShaderGenerator->createSubRenderState(Ogre::RTShader::PerPixelLighting::Type);
      schemRenderState->addTemplateSubRenderState(perPixelLightModel);
    }

    if (rtss_cache_enable_) {
      const std::string cacheDirectory = rtss_cache_dir_;

      if (!std::filesystem::exists(cacheDirectory)) {
        std::filesystem::create_directory(cacheDirectory);
      }

      mShaderGenerator->setShaderCachePath(cacheDirectory);
    } else {
      mShaderGenerator->setShaderCachePath("");
    }
  }
}
#endif
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::UpdateCursorStatus(Cursor cursor) {
  SDL_ShowCursor(cursor.show);
  SDL_SetWindowGrab(sdl_window_, static_cast<SDL_bool>(cursor.grab));
  SDL_SetRelativeMouseMode(static_cast<SDL_bool>(cursor.relative));
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::messageLogged(const std::string &message,
                                   Ogre::LogMessageLevel lml, bool maskDebug, const std::string &logName,
                                   bool &skipThisMessage) {
  switch (lml) {
    case Ogre::LML_WARNING: break;
    case Ogre::LML_NORMAL: break;
    case Ogre::LML_CRITICAL: break;
    case Ogre::LML_TRIVIAL: break;
  }
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::SetupOgreLog() {
  if (!global_log_enable_) {
    Ogre::LogManager *logger = new Ogre::LogManager();
    logger->createLog(ogre_log_name_, true, false, true);
    Ogre::LogManager::getSingleton().getDefaultLog()->addListener(this);
    Ogre::LogManager::getSingleton().setLogDetail(Ogre::LL_BOREME);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::Setup() {
  ShaderResolver::FixMaterial("BaseWhite");
  ShaderResolver::FixMaterial("BaseWhiteNoLighting");

  if (sdl_window_) {
    if (window_position_.f) {
      SDL_SetWindowSize(sdl_window_, actual_monitor_size_.w, actual_monitor_size_.h);
      SDL_WarpMouseGlobal(0, 0);
      SDL_SetWindowFullscreen(sdl_window_, SDL_WINDOW_FULLSCREEN);
      SDL_WarpMouseGlobal(0, 0);
      SDL_SetWindowSize(sdl_window_, window_position_.w, window_position_.h);
      SDL_WarpMouseGlobal(0, 0);
      ogre_render_window_->setFullscreen(true, window_position_.w, window_position_.h);
      SDL_WarpMouseGlobal(0, 0);
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::Reset() {
  RestoreScreenSize();
  SDL_DestroyWindow(sdl_window_);
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::RestoreScreenSize() {
  if (window_position_.f) {
    SDL_WarpMouseGlobal(0, 0);
    SDL_SetWindowFullscreen(sdl_window_, SDL_FALSE);
    SDL_WarpMouseGlobal(0, 0);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::SetupGlobal() {
  SetupConfigManager();
  SetupOgreLog();
  SetupPath();
  SetupInputs();
  SetupSDL();
  SetupOGRE();
#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  if (rtss_enable_) {
    SetupRTSS();
  }
#endif

#ifdef OGRE_BUILD_COMPONENT_RTSHADERSYSTEM
  if (rtss_enable_) {
    SetupShaderResolver();
  }
#endif

  InitGeneralResources();

  StaticForestManager::GetSingleton().SetupGlobal();
  StaticForestManager::GetSingleton().Setup();

  PagedForestManager::GetSingleton().SetupGlobal();
  PagedForestManager::GetSingleton().Setup();

  if (physics_enable_) {
    PhysicsManager::GetSingleton().SetupGlobal();
    PhysicsManager::GetSingleton().Setup();
  }

  if (sound_enable_) {
    SoundManager::GetSingleton().SetupGlobal();
    SoundManager::GetSingleton().Setup();
  }

  CompositorManager::GetSingleton().SetupGlobal();
  CompositorManager::GetSingleton().Setup();

  SetupOgreScenePreconditions();

  DotSceneLoaderB::GetSingleton().SetupGlobal();
  DotSceneLoaderB::GetSingleton().Setup();

  CubeMapCamera::GetSingleton().SetupGlobal();
//  CubeMapCamera::GetSingleton().Setup();

  ReflectionCamera::GetSingleton().SetupGlobal();
//  ReflectionCamera::GetSingleton().Setup();

  CeguiOverlayManager::GetSingleton().SetupGlobal();
  CeguiOverlayManager::GetSingleton().Setup();
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::ResetGlobals() {
  CompositorManager::GetSingleton().ResetGlobal();
  CompositorManager::GetSingleton().Reset();

  StaticForestManager::GetSingleton().ResetGlobal();
  StaticForestManager::GetSingleton().Reset();

  PagedForestManager::GetSingleton().ResetGlobal();
  PagedForestManager::GetSingleton().Reset();

  if (physics_enable_) {
    PhysicsManager::GetSingleton().ResetGlobal();
    PhysicsManager::GetSingleton().Reset();
  }

  if (sound_enable_) {
    SoundManager::GetSingleton().ResetGlobal();
    SoundManager::GetSingleton().Reset();
  }

  DotSceneLoaderB::GetSingleton().ResetGlobal();
  DotSceneLoaderB::GetSingleton().Reset();

  CubeMapCamera::GetSingleton().ResetGlobal();
  CubeMapCamera::GetSingleton().Reset();

  ReflectionCamera::GetSingleton().ResetGlobal();
  ReflectionCamera::GetSingleton().Reset();

  CeguiOverlayManager::GetSingleton().ResetGlobal();
  CeguiOverlayManager::GetSingleton().Reset();
}
//----------------------------------------------------------------------------------------------------------------------
void ContextManager::Render() {
  ogre_root_->renderOneFrame();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  SDL_GL_SwapWindow(sdl_window_);
#endif
}
//----------------------------------------------------------------------------------------------------------------------
const std::shared_ptr<Ogre::ShadowCameraSetup> &ContextManager::GetOgreShadowCameraSetup() const {
  return ogre_shadow_camera_setup_;
}
bool ContextManager::IsPhysicsEnable() const {
  return physics_enable_;
}
bool ContextManager::IsSoundEnable() const {
  return sound_enable_;
}

} //namespace Context
