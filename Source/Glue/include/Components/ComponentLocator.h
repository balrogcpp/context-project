// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Config.h"
#include "Engine.h"
#include "PhysicalInput/InputHandler.h"

namespace Glue {

class Render;
class Window;
class Compositor;
class PhysicsSystem;
class Audio;
class Overlay;
class DotSceneLoaderB;
class InputSequencer;

InputHandler& GetIo();
Config& GetConf();
Engine& GetEngine();
Render& GetRS();
Window& GetWindow();
Compositor& GetCompositor();
PhysicsSystem& GetPhysics();
Audio& GetAudio();
Overlay& GetOverlay();
DotSceneLoaderB& GetLoader();

}  // namespace Glue
