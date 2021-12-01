// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "RenderSystem.h"
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#include <RenderSystems/GLES2/OgreGLES2RenderSystem.h>
#endif

using namespace std;

namespace glue {

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitOgreRenderSystemGLES2() {
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
  Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLES2RenderSystem());
#endif
#endif
}

}  // namespace glue
