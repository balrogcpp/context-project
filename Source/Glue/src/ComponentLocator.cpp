// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "ComponentLocator.h"
#include "Engine.h"
#include "InputHandler.h"

namespace Glue {

InputHandler& GetIo() {
  static auto& InputComponentPtr = *Engine::GetInstance().io;
  return InputComponentPtr;
}

Config& GetConf() {
  static auto& ConfigPtr = *Engine::GetInstance().config;
  return ConfigPtr;
}

Engine& GetEngine() {
  static auto& EnginePtr = Engine::GetInstance();
  return EnginePtr;
}

RenderSystem& GetRS() {
  static auto& RenderComponentPtr = *Engine::GetInstance().rs;
  return RenderComponentPtr;
}

Window& GetWindow() {
  static auto& WindowPtr = GetRS().GetWindow();
  return WindowPtr;
}

Compositor& GetCompositor() {
  static auto& CompositorComponentPtr = GetRS().GetCompositor();
  return CompositorComponentPtr;
}

PhysicsSystem& GetPhysics() {
  static auto& PhysicsComponentPtr = *Engine::GetInstance().ps;
  return PhysicsComponentPtr;
}

AudioSystem& GetAudio() {
  static auto& AudioComponentPtr = *Engine::GetInstance().as;
  return AudioComponentPtr;
}

Overlay& GetOverlay() {
  static auto& OverlayComponentPtr = *Engine::GetInstance().overlay;
  return OverlayComponentPtr;
}

DotSceneLoaderB& GetLoader() {
  static auto& LoaderPtr = *Engine::GetInstance().loader;
  return LoaderPtr;
}

}  // namespace Glue
