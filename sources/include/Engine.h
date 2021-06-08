// This source file is part of context-project
// Created by Andrew Vasiliev

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

namespace xio {

class Engine : public LazySingleton<Engine> {
 public:
  Engine();
  virtual ~Engine();

  void InitComponents();
  void Capture();
  void Pause();
  void InMenu();
  void OffMenu();
  void Resume();
  void Cleanup();
  void Refresh();
  void Update(float time);
  void RenderOneFrame();
  void RegComponent(view_ptr<System> component);

 private:
  std::unique_ptr<InputHandler> io_;
  std::unique_ptr<Config> conf_;
  std::unique_ptr<RenderSystem> renderer_;
  std::unique_ptr<PhysicsSystem> physics_;
  std::unique_ptr<AudioSystem> audio_;
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

}  // namespace xio
