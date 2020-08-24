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

#include "pcheader.h"

#include "Renderer.h"
#include "Exception.h"
#include "Storage.h"
#include "DotSceneLoaderB.h"
#include "Forest.h"

namespace Context {
Renderer::Renderer() {
  root_ = new Ogre::Root("", "", "");

  bool global_sso_enable_ = true;
  bool global_octree_enable_ = true;
  bool global_stbi_enable_ = true;
  bool global_freeimage_enable_ = false;
  bool global_particlefx_enable_ = true;

#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
  auto *mGlesRenderSystem = new Ogre::GLES2RenderSystem();
  Ogre::Root::getSingleton().setRenderSystem(mGlesRenderSystem);
#else
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  auto *mGL3PlusRenderSystem = new Ogre::GL3PlusRenderSystem();
  if (global_sso_enable_)
    mGL3PlusRenderSystem->setConfigOption("Separate Shader Objects", "Yes");
  else
    mGL3PlusRenderSystem->setConfigOption("Separate Shader Objects", "No");
  Ogre::Root::getSingleton().setRenderSystem(mGL3PlusRenderSystem);
#else
  #ifdef OGRE_BUILD_RENDERSYSTEM_GL
  auto *mGLRenderSystem = new Ogre::GLRenderSystem();
  Ogre::Root::getSingleton().setRenderSystem(mGLRenderSystem);
#endif
#endif //OGRE_BUILD_RENDERSYSTEM_GL3PLUS
#endif //OGRE_BUILD_RENDERSYSTEM_GLES2
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
  root_->initialise(false);
  Ogre::NameValuePairList params;

  SDL_SysWMinfo info = window_.GetInfo();
//  window_.Resize(conf.GetInt("window_width"),
//                 conf.GetInt("window_high"));
//  if (conf.GetBool("window_fullscreen"))
//    window_.Fullscreen(true);
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

  bool graphics_vsync_ = true;
  bool graphics_gamma_enable_ = false;
  int graphics_fsaa_ = 0;
  int graphics_msaa_ = 0;
//  conf.Assign(graphics_vsync_, "graphics_vsync");
//  conf.Assign(graphics_gamma_enable_, "graphics_gamma");
//  conf.Assign(graphics_fsaa_, "graphics_fsaa");
//  conf.Assign(graphics_msaa_, "graphics_msaa");

  params["vsync"] = graphics_vsync_ ? true_str : false_str;
  params["gamma"] = graphics_gamma_enable_ ? true_str : false_str;
  params["FSAA"] = std::to_string(graphics_fsaa_);
  params["MSAA"] = std::to_string(graphics_msaa_);

  auto *ogre_render_window_ = root_->createRenderWindow(window_.GetCaption(), window_.GetSize().first, \
                       window_.GetSize().second, false, &params);

#ifdef OGRE_BUILD_PLUGIN_OCTREE
  if (global_octree_enable_) {
    scene_ = root_->createSceneManager("OctreeSceneManager", "Default");
  } else {
    scene_ = root_->createSceneManager("Default");
  }
#else
  scene_mgr_ = ogre_root->createSceneManager();
#endif

  auto *camera = scene_->createCamera("Default");
  auto *renderTarget = root_->getRenderTarget(window_.GetCaption());

  auto *viewport = renderTarget->addViewport(camera);
  viewport->setBackgroundColour(Ogre::ColourValue::Black);
}
Renderer::~Renderer() {}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::CreateCamera() {
  Ogre::Camera *camera = nullptr;

  if (!scene_->hasCamera("Default")) {
    camera = scene_->createCamera("Default");
    auto *renderTarget = root_->getRenderTarget(window_.GetCaption());
    renderTarget->removeViewport(0);
    auto *viewport = renderTarget->addViewport(camera);
    viewport->setBackgroundColour(Ogre::ColourValue::Black);
  } else {
    camera = scene_->getCamera("Default");
  }

  if (camera) {
    camera->setNearClipDistance(0.01f);
    camera->setFarClipDistance(10000.0f);
  }

  camera->setAspectRatio(window_.GetRatio());

  scene_->setSkyBoxEnabled(false);
  scene_->setSkyDomeEnabled(false);
  scene_->setAmbientLight(Ogre::ColourValue::Black);
}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateParams() {
  CreateCamera();

  std::string graphics_filtration_ = "bilinear";
  int graphics_anisotropy_level_ = 4;
  int graphics_mipmap_count_ = 10;
//  auto &conf = ConfiguratorJson::Instance();
//  conf.Assign(graphics_filtration_, "graphics_filtration");
//  conf.Assign(graphics_anisotropy_level_, "graphics_anisotropy_level");
//  conf.Assign(graphics_mipmap_count_, "graphics_mipmap_count");
  auto *camera = scene_->getCamera("Default");

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

//  conf.Assign(graphics_shadows_enable_, "graphics_shadows_enable");
//  conf.Assign(graphics_shadows_pssm_0_, "graphics_shadows_pssm_0");
//  conf.Assign(graphics_shadows_pssm_1_, "graphics_shadows_pssm_1");
//  conf.Assign(graphics_shadows_pssm_2_, "graphics_shadows_pssm_2");
//  conf.Assign(graphics_shadows_split_auto_, "graphics_shadows_split_auto");
//  conf.Assign(graphics_shadows_texture_format_, "graphics_shadows_texture_format");
//  conf.Assign(graphics_shadows_texture_resolution_, "graphics_shadows_texture_resolution");
//  conf.Assign(graphics_shadows_split_padding_, "graphics_shadows_split_padding");
//  conf.Assign(graphics_shadows_texture_count_, "graphics_shadows_texture_count");
//  conf.Assign(graphics_shadows_far_distance_, "graphics_shadows_far_distance");
//  conf.Assign(graphics_shadows_self_shadow_, "graphics_shadows_self_shadow");
//  conf.Assign(graphics_shadows_back_faces_, "graphics_shadows_back_faces");
//  conf.Assign(graphics_shadows_caster_material_, "graphics_shadows_caster_material");
//  conf.Assign(graphics_shadows_receiver_material_, "graphics_shadows_receiver_material");
//  conf.Assign(graphics_shadows_tecnique_, "graphics_shadows_tecnique");
//  conf.Assign(graphics_shadows_lighting_, "graphics_shadows_lighting");
//  conf.Assign(graphics_shadows_projection_, "graphics_shadows_projection");
//  conf.Assign(graphics_shadows_material_, "graphics_shadows_material");
//  conf.Assign(graphics_shadows_integrated_, "graphics_shadows_integrated");

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

    scene_->setShadowTechnique(static_cast<Ogre::ShadowTechnique>(shadow_technique));

    if (graphics_shadows_material_ != "none") {
      Ogre::PixelFormat texture_type;

      if (graphics_shadows_texture_format_ == "F32_R")
        texture_type = Ogre::PF_FLOAT32_R;
      else if (graphics_shadows_texture_format_ == "F16_R")
        texture_type = Ogre::PF_FLOAT16_R;
      else if (graphics_shadows_texture_format_ == "F32_GR")
        texture_type = Ogre::PF_FLOAT32_GR;
      else if (graphics_shadows_texture_format_ == "F16_GR")
        texture_type = Ogre::PF_FLOAT16_GR;
      else if (graphics_shadows_texture_format_ == "DEPTH16")
        texture_type = Ogre::PF_DEPTH16;
      else if (graphics_shadows_texture_format_ == "DEPTH32")
        texture_type = Ogre::PF_DEPTH32;
      else if (graphics_shadows_texture_format_ == "DEPTH32F")
        texture_type = Ogre::PF_DEPTH32F;
      else
        texture_type = Ogre::PF_DEPTH32;

      scene_->setShadowTextureSettings(graphics_shadows_texture_resolution_,
                                       graphics_shadows_texture_count_,
                                       texture_type,
                                       0,
                                       2);

      scene_->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);
      scene_->setShadowTextureCountPerLightType(Ogre::Light::LT_SPOTLIGHT, 3);
      scene_->setShadowTextureCountPerLightType(Ogre::Light::LT_POINT, 6);
      scene_->setShadowTextureSelfShadow(graphics_shadows_self_shadow_);
      scene_->setShadowCasterRenderBackFaces(graphics_shadows_back_faces_);
      scene_->setShadowFarDistance(graphics_shadows_far_distance_);
      scene_->setShadowDirectionalLightExtrusionDistance(graphics_shadows_far_distance_);

      if (!graphics_shadows_caster_material_.empty()) {
        auto passCaterMaterial = Ogre::MaterialManager::getSingleton().getByName(graphics_shadows_caster_material_);
        scene_->setShadowTextureCasterMaterial(passCaterMaterial);
      }

      if (!graphics_shadows_receiver_material_.empty()) {
        auto
            passReceiverMaterial = Ogre::MaterialManager::getSingleton().getByName(graphics_shadows_receiver_material_);
        scene_->setShadowTextureReceiverMaterial(passReceiverMaterial);
      }

      const int pssm_splits_ = 3;

      auto pssm_setup_ = std::make_shared<Ogre::PSSMShadowCameraSetup>();

        if (graphics_shadows_split_auto_) {
          pssm_setup_->calculateSplitPoints(pssm_splits_, camera->getNearClipDistance(),scene_->getShadowFarDistance());
        } else {
          Ogre::PSSMShadowCameraSetup::SplitPointList split_points =
              {camera->getNearClipDistance(), 10.0, 20.0, scene_->getShadowFarDistance()};
          pssm_setup_->setSplitPoints(split_points);
        }

        if (graphics_shadows_split_padding_ == 0) {
          pssm_setup_->setSplitPadding(1.0);
        } else {
          pssm_setup_->setSplitPadding(graphics_shadows_split_padding_);
        }

      pssm_setup_->setOptimalAdjustFactor(0, graphics_shadows_pssm_0_);
      pssm_setup_->setOptimalAdjustFactor(1, graphics_shadows_pssm_1_);
      pssm_setup_->setOptimalAdjustFactor(2, graphics_shadows_pssm_2_);
      scene_->setShadowCameraSetup(pssm_setup_);
    }
  } else {
    scene_->setShadowTechnique(Ogre::SHADOWTYPE_NONE);
  }

  bool graphics_debug_show_wireframe_ = false;
//  conf.Assign(graphics_debug_show_wireframe_, "graphics_debug_show_wireframe");

  if (graphics_debug_show_wireframe_) {
    camera->setPolygonMode(Ogre::PM_WIREFRAME);
  } else {
    camera->setPolygonMode(Ogre::PM_SOLID);
  }
}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::Render() {
  root_->renderOneFrame();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  window_.SwapBuffers();
#endif
}
} //namespace Context
