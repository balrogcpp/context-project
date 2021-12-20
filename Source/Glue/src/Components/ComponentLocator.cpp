// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Components/ComponentLocator.h"
#include "Engine.h"

namespace Glue {

Conf& GetConf() {
  static auto& ConfPtr = *Engine::GetInstance().ConfPtr;
  return ConfPtr;
}

Engine& GetEngine() {
  static auto& EnginePtr = Engine::GetInstance();
  return EnginePtr;
}

Physics& GetPhysics() {
  static auto& PhysicsComponentPtr = *Engine::GetInstance().PhysicsPtr;
  return PhysicsComponentPtr;
}

Sound& GetAudio() {
  static auto& AudioComponentPtr = *Engine::GetInstance().SoundPtr;
  return AudioComponentPtr;
}

DotSceneLoaderB& GetLoader() {
  static auto& LoaderPtr = *Engine::GetInstance().LoaderPtr;
  return LoaderPtr;
}

}  // namespace Glue
