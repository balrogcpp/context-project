// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Config.h"
#include "Engine.h"

namespace Glue {

class Render;
class Window;
class Compositor;
class Physics;
class Audio;
class Overlay;
class DotSceneLoaderB;
class InputSequencer;

Config& GetConf();
Engine& GetEngine();
Render& GetRS();
Window& GetWindow();
Compositor& GetCompositor();
Physics& GetPhysics();
Audio& GetAudio();
Overlay& GetOverlay();
DotSceneLoaderB& GetLoader();

}  // namespace Glue
