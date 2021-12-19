// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Conf.h"
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

Conf& GetConf();
Engine& GetEngine();
Physics& GetPhysics();
Sound& GetAudio();
DotSceneLoaderB& GetLoader();

}  // namespace Glue
