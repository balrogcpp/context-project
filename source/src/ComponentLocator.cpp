// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "ComponentLocator.h"
#include "Engine.h"
#include "InputHandler.h"

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
InputHandler& GetIo() {
  static auto& io = *Engine::GetInstance().io_;
  return io;
}

//----------------------------------------------------------------------------------------------------------------------
Config& GetConf() {
  static auto& config = *Engine::GetInstance().config_;
  return config;
}

//----------------------------------------------------------------------------------------------------------------------
Engine& GetEngine() {
  static auto& engine = Engine::GetInstance();
  return engine;
}

//----------------------------------------------------------------------------------------------------------------------
RenderSystem& GetRS() {
  static auto& renderer = *Engine::GetInstance().rs_;
  return renderer;
}

//----------------------------------------------------------------------------------------------------------------------
Window& GetWindow() {
  static auto& window = GetRS().GetWindow();
  return window;
}

//----------------------------------------------------------------------------------------------------------------------
Compositor& GetCompositor() {
  static auto& compositor = GetRS().GetCompositor();
  return compositor;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsSystem& GetPhysics() {
  static auto& physics = *Engine::GetInstance().ps_;
  return physics;
}

//----------------------------------------------------------------------------------------------------------------------
AudioSystem& GetAudio() {
  static auto& audio = *Engine::GetInstance().as_;
  return audio;
}

//----------------------------------------------------------------------------------------------------------------------
Overlay& GetOverlay() {
  static auto& overlay = *Engine::GetInstance().overlay_;
  return overlay;
}

//----------------------------------------------------------------------------------------------------------------------
DotSceneLoaderB& GetLoader() {
  static auto& loader = *Engine::GetInstance().loader_;
  return loader;
}

}  // namespace glue
