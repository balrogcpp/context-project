// This source file is part of "glue project". Created by Andrew Vasiliev

#pragma once
#include "Config.h"
#include "Engine.h"
#include "InputHandler.h"

namespace Glue {

class RenderSystem;
class Window;
class Compositor;
class PhysicsSystem;
class AudioSystem;
class Overlay;
class DotSceneLoaderB;
class InputSequencer;

InputHandler& GetIo();
Config& GetConf();
Engine& GetEngine();
RenderSystem& GetRS();
Window& GetWindow();
Compositor& GetCompositor();
PhysicsSystem& GetPhysics();
AudioSystem& GetAudio();
Overlay& GetOverlay();
DotSceneLoaderB& GetLoader();

}  // namespace Glue
