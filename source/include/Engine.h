// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "AppState.h"
#include "AudioSystem.h"
#include "DotSceneLoaderB.h"
#include "Engine.h"
#include "InputHandler.h"
#include "Overlay.h"
#include "PhysicsSystem.h"
#include "RenderSystem.h"
#include "Singleton.h"
#include "view_ptr.h"
#include <memory>
#include <vector>

namespace glue {

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
  void RegSystem(view_ptr<System> system);

 protected:
  std::unique_ptr<InputHandler> io_;
  std::unique_ptr<Config> config_;
  std::unique_ptr<RenderSystem> rs_;
  std::unique_ptr<PhysicsSystem> ps_;
  std::unique_ptr<AudioSystem> as_;
  std::unique_ptr<Overlay> overlay_;
  std::unique_ptr<DotSceneLoaderB> loader_;
  std::vector<view_ptr<System>> components_;

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

}  // namespace glue
