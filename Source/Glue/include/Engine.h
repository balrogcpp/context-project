// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppState.h"
#include "AudioSystem.h"
#include "DotSceneLoaderB.h"
#include "LazySingleton.h"
#include "Engine.h"
#include "InputHandler.h"
#include "Overlay.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include <memory>
#include <vector>

namespace Glue {

class Engine : public LazySingleton<Engine> {
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
  void Update(float time);
  void RenderOneFrame();
  void RegSystem(Component* system);

 protected:
  std::unique_ptr<InputHandler> io;
  std::unique_ptr<Config> config;
  std::unique_ptr<RenderSystem> rs;
  std::unique_ptr<PhysicsSystem> ps;
  std::unique_ptr<AudioSystem> as;
  std::unique_ptr<Overlay> overlay;
  std::unique_ptr<DotSceneLoaderB> loader;
  std::vector<Component*> ComponentList;

  friend InputHandler& GetIo();
  friend Config& GetConf();
  friend RenderSystem& GetRS();
  friend Window& GetWindow();
  friend Compositor& GetCompositor();
  friend PhysicsSystem& GetPhysics();
  friend AudioSystem& GetAudio();
  friend Overlay& GetOverlay();
  friend DotSceneLoaderB& GetLoader();
};

}  // namespace Glue
