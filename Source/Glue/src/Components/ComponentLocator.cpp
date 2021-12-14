// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Components/ComponentLocator.h"
#include "Engine.h"

namespace Glue {

Config& GetConf() {
  static auto& ConfigPtr = *Engine::GetInstance().ConfPtr;
  return ConfigPtr;
}

Engine& GetEngine() {
  static auto& EnginePtr = Engine::GetInstance();
  return EnginePtr;
}

//Render& GetRS() {
//  static auto& RenderComponentPtr = *Engine::GetInstance().RS;
//  return RenderComponentPtr;
//}

//Window& GetWindow() {
//  static auto& WindowPtr = GetRS().GetWindow();
//  return WindowPtr;
//}

Physics& GetPhysics() {
  static auto& PhysicsComponentPtr = *Engine::GetInstance().ps;
  return PhysicsComponentPtr;
}

Audio& GetAudio() {
  static auto& AudioComponentPtr = *Engine::GetInstance().as;
  return AudioComponentPtr;
}

DotSceneLoaderB& GetLoader() {
  static auto& LoaderPtr = *Engine::GetInstance().loader;
  return LoaderPtr;
}

}  // namespace Glue
