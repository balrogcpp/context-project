// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Engine.h"
/// \file Compilation failes with both GL3 and GLES2 headers included
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#include <RenderSystems/GLES2/OgreGLES2RenderSystem.h>
#endif

using namespace std;

namespace Glue {

void Engine::InitOgreRenderSystemGLES2() {
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
  Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLES2RenderSystem());
#endif
#endif
}

}  // namespace Glue