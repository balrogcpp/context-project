// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Components/ComponentLocator.h"

namespace Glue {

//Conf& GetConf() {
//  static auto& ConfPtr = *Engine::GetInstance().ConfPtr;
//  return ConfPtr;
//}

Engine& GetEngine() {
  static auto& EnginePtr = Engine::GetInstance();
  return EnginePtr;
}

Physics& GetPhysics() { return *GetComponent<Physics>(); }

Sound& GetAudio() { return *GetComponent<Sound>(); }

Scene& GetScene() { return *GetComponent<Scene>(); }

}  // namespace Glue
