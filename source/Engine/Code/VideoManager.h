/// created by Andrey Vasiliev

#pragma once
#include "SDLListener.h"
#include "System.h"
#include "Window.h"
#include <Ogre.h>
#include <OgreImGuiOverlay.h>

namespace Glue {
class VideoManager final : public WindowListener, public System<VideoManager> {
 protected:
  class ShaderResolver;

 public:
  VideoManager();
  virtual ~VideoManager();

  void RenderFrame();
  Window& GetWindow(int number = 0);
  Window& GetMainWindow();
  void ShowWindow(bool show, int index = 0);

  void UnloadResources();
  void ClearScene();

  void GamepadEnable(bool enable);
  void ShowOverlay(bool show);
  ImFont* AddFont(const std::string& name, const int size = 0, const std::string& group = Ogre::RGN_AUTODETECT, const ImFontConfig* cfg = nullptr,
                  const ImWchar* ranges = nullptr);

  void EnableShadows(bool enable);
  bool IsShadowEnabled();
  void SetShadowTexSize(unsigned short size);
  unsigned short GetShadowTexSize();
  void SetTexFiltering(unsigned int type, int anisotropy);
  std::vector<float> GetPSSMSplitPoints();

 protected:
  void MakeWindow();
  void InitOgreRoot();
  void InitSDL();
  void InitOgreRTSS();
  void InitOgreOverlay();
  void LoadResources();
  void InitOgre();
  void CheckGPU();

  /// System impl
  void OnSetUp() override;
  void OnClean() override;
  void OnUpdate(float time) override;

  /// WindowListener impl
  void OnEvent(const SDL_Event& event) override;

  Window* mainWindow = nullptr;
  std::vector<Window> windowList;
  std::string ogreLogFile;
  unsigned short ogreMinLogLevel;
  std::unique_ptr<Ogre::LogManager> ogreLogManager;
  std::unique_ptr<ShaderResolver> shaderResolver;
  Ogre::ShadowTechnique shadowTechnique;
  Ogre::Real shadowFarDistance;
  bool shadowEnabled;
  unsigned short shadowTexSize;
  std::shared_ptr<Ogre::PSSMShadowCameraSetup> pssmSetup;
  std::vector<Ogre::Real> pssmSplitPointList;
  int pssmSplitCount;
  bool gamepadSupport;

  Ogre::Root* ogreRoot = nullptr;
  Ogre::SceneManager* ogreSceneManager = nullptr;
  Ogre::RenderWindow* ogreWindow = nullptr;
  Ogre::Camera* ogreCamera = nullptr;
  Ogre::Viewport* ogreViewport = nullptr;
  Ogre::ImGuiOverlay* imguiOverlay = nullptr;
};
}  // namespace Glue
