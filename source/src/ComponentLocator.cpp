// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "ComponentLocator.h"
#include "Engine.h"
#include "InputHandler.h"

namespace glue {

InputHandler& GetIo() {
  static auto& io = *Engine::GetInstance().io;
  return io;
}

Config& GetConf() {
  static auto& config = *Engine::GetInstance().config;
  return config;
}

Engine& GetEngine() {
  static auto& engine = Engine::GetInstance();
  return engine;
}

RenderSystem& GetRS() {
  static auto& renderer = *Engine::GetInstance().rs;
  return renderer;
}

Window& GetWindow() {
  static auto& window = GetRS().GetWindow();
  return window;
}

Compositor& GetCompositor() {
  static auto& compositor = GetRS().GetCompositor();
  return compositor;
}

PhysicsSystem& GetPhysics() {
  static auto& physics = *Engine::GetInstance().ps;
  return physics;
}

AudioSystem& GetAudio() {
  static auto& audio = *Engine::GetInstance().as;
  return audio;
}

Overlay& GetOverlay() {
  static auto& overlay = *Engine::GetInstance().overlay;
  return overlay;
}

DotSceneLoaderB& GetLoader() {
  static auto& loader = *Engine::GetInstance().loader;
  return loader;
}

}  // namespace glue
