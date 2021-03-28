//MIT License
//
//Copyright (c) 2021 Andrew Vasiliev
//
//Permission is hereby granted, free of charge, to any person obtaining a copy
//of this software and associated documentation files (the "Software"), to deal
//in the Software without restriction, including without limitation the rights
//to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//copies of the Software, and to permit persons to whom the Software is
//furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all
//copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//SOFTWARE.

#include "pcheader.h"
#include "Renderer.h"
#include "Assets.h"
#include "RtssUtils.h"
#include "Exception.h"

#undef OGRE_BUILD_RENDERSYSTEM_GLES2

#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#include <RenderSystems/GLES2/OgreGLES2RenderSystem.h>
#elif defined OGRE_BUILD_RENDERSYSTEM_GL3PLUS
#include <RenderSystems/GL3Plus/OgreGL3PlusRenderSystem.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_OCTREE
#include <Plugins/OctreeSceneManager/OgreOctreeSceneManager.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_PFX
#include <Plugins/ParticleFX/OgreParticleFXPlugin.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_STBI
#include <Plugins/STBICodec/OgreSTBICodec.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_FREEIMAGE
#include <Plugins/FreeImageCodec/OgreFreeImageCodec.h>
#include <Plugins/FreeImageCodec/OgreFreeImageCodecExports.h>
#endif
#ifdef OGRE_BUILD_PLUGIN_ASSIMP
#include <Plugins/Assimp/OgreAssimpLoader.h>
#endif
//#ifdef OGRE_BUILD_COMPONENT_VOLUME
//#include <Volume/OgreVolumeChunk.h>
//#include <Volume/OgreVolumeCSGSource.h>
//#include <Volume/OgreVolumeCacheSource.h>
//#include <Volume/OgreVolumeTextureSource.h>
//#endif


#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include <OgreArchiveFactory.h>
#include <OgreFileSystem.h>
#include <OgreZip.h>

#include <android_native_app_glue.h>
#include <android/configuration.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/input.h>
#include <android/input.h>
#include <android/sensor.h>
#include <android/log.h>
#include <jni.h>
#endif


using namespace std;
namespace fs = filesystem;

namespace xio {
Renderer::Renderer(int w, int h, bool f) {
  root_ = new Ogre::Root("");


#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
  Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLES2RenderSystem());
#elif defined OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  auto *gl3plus_render_system = new Ogre::GL3PlusRenderSystem();
  gl3plus_render_system->setConfigOption("Separate Shader Objects", "Yes");

  Ogre::Root::getSingleton().setRenderSystem(gl3plus_render_system);
#endif
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  Ogre::Root::getSingleton().addSceneManagerFactory(new Ogre::OctreeSceneManagerFactory());
#endif // OGRE_BUILD_PLUGIN_OCTREE
#ifdef OGRE_BUILD_PLUGIN_PFX
  Ogre::Root::getSingleton().installPlugin(new Ogre::ParticleFXPlugin());
#endif // OGRE_BUILD_PLUGIN_PFX
#ifdef OGRE_BUILD_PLUGIN_STBI
  Ogre::Root::getSingleton().installPlugin(new Ogre::STBIPlugin());
#endif // OGRE_BUILD_PLUGIN_STBI
#ifdef OGRE_BUILD_PLUGIN_FREEIMAGE
  Ogre::Root::getSingleton().installPlugin(new Ogre::FreeImagePlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_ASSIMP
  Ogre::Root::getSingleton().installPlugin(new Ogre::AssimpPlugin());
#endif
#ifdef OGRE_BUILD_PLUGIN_OCTREE
  scene_ = root_->createSceneManager("OctreeSceneManager", "Default");
#else
  ogre_scene_ = root_->createSceneManager(Ogre::ST_GENERIC, "Default");
#endif

  root_->initialise(false);

  window_ = make_unique<Window>(w, h, f);


  Ogre::NameValuePairList params;

  #if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  SDL_SysWMinfo info = window_->GetInfo();
  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.win.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
  SDL_SysWMinfo info = window_->GetInfo();
  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.x11.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
  SDL_SysWMinfo info = window_->GetInfo();
  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.cocoa.window));
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
  SDL_SysWMinfo info = window_->GetInfo();
  JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

  jclass class_activity       = env->FindClass("android/app/Activity");
  jclass class_resources      = env->FindClass("android/content/res/Resources");
  jmethodID method_get_resources      = env->GetMethodID(class_activity, "getResources", "()Landroid/content/res/Resources;");
  jmethodID method_get_assets         = env->GetMethodID(class_resources, "getAssets", "()Landroid/content/res/AssetManager;");
  jobject raw_activity = (jobject)SDL_AndroidGetActivity();
  jobject raw_resources = env->CallObjectMethod(raw_activity, method_get_resources);
  jobject raw_asset_manager = env->CallObjectMethod(raw_resources, method_get_assets);
  AAssetManager* asset_manager = AAssetManager_fromJava(env, raw_asset_manager);
  AConfiguration* mAConfig = AConfiguration_new();
  AConfiguration_fromAssetManager(mAConfig, asset_manager);

  Ogre::ArchiveManager::getSingleton().addArchiveFactory(new Ogre::APKFileSystemArchiveFactory(asset_manager));
  Ogre::ArchiveManager::getSingleton().addArchiveFactory(new Ogre::APKZipArchiveFactory(asset_manager));

  SDL_GLContext m_sdl_gl_context = SDL_GL_GetCurrentContext();

  jclass class_sdl_activity   = env->FindClass("org/libsdl/app/SDLActivity");
  jmethodID method_get_native_surface = env->GetStaticMethodID(class_sdl_activity, "getNativeSurface", "()Landroid/view/Surface;");
  jobject raw_surface = env->CallStaticObjectMethod(class_sdl_activity, method_get_native_surface);
  ANativeWindow *native_window = ANativeWindow_fromSurface(env, raw_surface);

  params["currentGLContext"]     = "true";
//  params["preserveContext"] = "true";
  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(native_window));
//  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(info.info.android.window));
  params["externalGLContext"]    = to_string(reinterpret_cast<size_t>(m_sdl_gl_context));

//  params["externalWindowHandle"] = to_string(reinterpret_cast<size_t>(native_window));
//  params["androidConfig"] = to_string(reinterpret_cast<size_t>(mAConfig));
//  params["preserveContext"] = "true";
//  params["currentGLContext"] = "true";
#endif

  const char true_str[] = "true";
  const char false_str[] = "false";

  bool vsync_ = conf_->Get<bool>("graphics_vsync");
  bool gamma_ = conf_->Get<bool>("graphics_gamma");
  int fsaa_ = conf_->Get<int>("graphics_fsaa");

  params["vsync"] = vsync_ ? true_str : false_str;
  params["gamma"] = gamma_ ? true_str : false_str;
  params["FSAA"] = to_string(fsaa_);

  render_window_ = Ogre::Root::getSingleton().createRenderWindow("MyDemo", 0, 0, false, &params);

  //Camera block
  camera_ = scene_->createCamera("Default");
  auto *renderTarget = root_->getRenderTarget(render_window_->getName());
  viewport_ = renderTarget->addViewport(camera_.get());
  viewport_->setBackgroundColour(Ogre::ColourValue::Black);
  camera_->setAspectRatio(static_cast<float>(viewport_->getActualWidth()) / static_cast<float>(viewport_->getActualHeight()));
  camera_->setAutoAspectRatio(true);

  //Resource block
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
  Assets::InitGeneralResources("programs", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "resources.list");
  Assets::InitGeneralResources("assets", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, "resources.list");
#else
  Ogre::ResourceGroupManager& resGroupMan = Ogre::ResourceGroupManager::getSingleton();
  Ogre::String defResGroup = Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME;
  resGroupMan.addResourceLocation("/", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/common", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/rtss", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/particles", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/overlay", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/gorilla", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/compositor", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/pbr", "APKFileSystem", defResGroup);

  resGroupMan.addResourceLocation("/1", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/bog", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/foam-grip", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/helmet", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/material", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/models", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/protocol", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/sinbad", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/sounds", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/terrain", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/Titanium-scuffed", "APKFileSystem", defResGroup);
  resGroupMan.addResourceLocation("/vegetation", "APKFileSystem", defResGroup);
#endif

  //RTSS block
  xio::InitRtss();
  Assets::LoadResources();
  xio::CreateRtssShaders();
//  xio::InitInstansing();

  //Shadow block
  shadow_ = make_unique<ShadowSettings>();
  //Compositor block
  compositor_ = make_unique<Compositor>();

  compositor_->EnableEffect("ssao", conf_->Get<bool>("compositor_use_ssao"));
  compositor_->EnableEffect("bloom", conf_->Get<bool>("compositor_use_bloom"));
  compositor_->EnableEffect("hdr", conf_->Get<bool>("compositor_use_hdr"));
  compositor_->EnableEffect("motion", conf_->Get<bool>("compositor_use_motion"));
  compositor_->Init();
//  xio::InitPssm(shadow_->GetSplitPoints());
}

//----------------------------------------------------------------------------------------------------------------------
Renderer::~Renderer() {

}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::Cleanup() {

}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::Pause() {

}
//----------------------------------------------------------------------------------------------------------------------
void Renderer::Resume() {

}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::Update(float time) {
  compositor_->Update(time);
};

//----------------------------------------------------------------------------------------------------------------------
void Renderer::Refresh() {
  shadow_->UpdateParams();
  compositor_->Init();
  Ogre::MaterialManager::getSingleton().load("Gorilla2D", Ogre::ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateShadow(bool enable, float far_distance, int tex_size, int tex_format) {
  shadow_->UpdateParams(enable, far_distance, tex_size, tex_format);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::UpdateParams(Ogre::TextureFilterOptions filtering, int anisotropy) {
  Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(Ogre::MIP_UNLIMITED);
  Ogre::MaterialManager::getSingleton().setDefaultTextureFiltering(filtering);
  Ogre::MaterialManager::MaterialManager::getSingleton().setDefaultAnisotropy(anisotropy);
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::Resize(int w, int h, bool f) {
//  if (f) {
//    window_->SetFullscreen(f);
//    render_window_->resize(window_->GetSize().first, window_->GetSize().second);
//    render_window_->setFullscreen(f, window_->GetSize().first, window_->GetSize().second);
//  } else {
//    window_->Resize(w, h);
//    render_window_->resize(w, h);
//  }
}

//----------------------------------------------------------------------------------------------------------------------
void Renderer::RenderOneFrame() {
  root_->renderOneFrame();
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
  window_->SwapBuffers();
#endif
}

} //namespace
