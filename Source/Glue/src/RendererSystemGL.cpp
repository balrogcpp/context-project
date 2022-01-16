// This source file is part of "glue project". Created by Andrey Vasiliev

#include "PCHeader.h"
#include "Engine.h"
/// \file Compilation failes with both GL3 and GLES2 headers included

#ifdef OGRE_BUILD_RENDERSYSTEM_GL

#include <RenderSystems/GL/OgreGLRenderSystem.h>

using namespace std;

namespace Glue {

void Engine::InitOgreRenderSystemGL() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLRenderSystem()); }

}  // namespace Glue

#endif
