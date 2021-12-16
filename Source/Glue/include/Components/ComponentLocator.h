// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Config.h"
#include "Engine.h"

namespace Glue {
class Compositor;
class Physics;
class Sound;
class Overlay;
class DotSceneLoaderB;
class InputSequencer;
}  // namespace Glue

namespace Glue {

Config& GetConf();
Engine& GetEngine();
Physics& GetPhysics();
Sound& GetAudio();
DotSceneLoaderB& GetLoader();

}  // namespace Glue
