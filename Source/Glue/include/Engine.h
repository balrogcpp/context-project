// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppState.h"
#include "Components/Audio.h"
#include "Components/DotSceneLoaderB.h"
#include "Components/Overlay.h"
#include "Components/Physics.h"
#include "Engine.h"
#include "LazySingleton.h"
#include "Render.h"
#include <memory>
#include <vector>

namespace Glue {

class Engine final : public LazySingleton<Engine> {
 public:
  Engine();
  virtual ~Engine();

  void InitSystems();
  void Capture();
  void Pause();
  void InMenu();
  void OffMenu();
  void Resume();
  void Cleanup();
  void Refresh();
  void Update(float PassedTime);
  void RenderOneFrame();
  void RegSystem(Component* system);

 private:
  //std::unique_ptr<InputHandler> io;
  std::unique_ptr<Config> config;
  std::unique_ptr<Render> rs;
  std::unique_ptr<Physics> ps;
  std::unique_ptr<Audio> as;
  std::unique_ptr<Overlay> overlay;
  std::unique_ptr<DotSceneLoaderB> loader;
  std::vector<Component*> ComponentList;

  friend Config& GetConf();
  friend Render& GetRS();
  friend Window& GetWindow();
  friend Compositor& GetCompositor();
  friend Physics& GetPhysics();
  friend Audio& GetAudio();
  friend Overlay& GetOverlay();
  friend DotSceneLoaderB& GetLoader();
};

}  // namespace Glue
