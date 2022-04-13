// This source file is part of Glue Engine. Created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include "PCHeader.h"
#include "Engine.h"
#ifdef OGRE_BUILD_RENDERSYSTEM_GL
#include <RenderSystems/GL/OgreGLRenderSystem.h>

using namespace std;

namespace Glue {

void Engine::InitOgreRenderSystemGL() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLRenderSystem()); }

}  // namespace Glue

#endif
