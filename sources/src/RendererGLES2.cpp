// This source file is part of context-project
// Created by Andrew Vasiliev

#include "RenderSystem.h"
#include "pcheader.h"

#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#include <RenderSystems/GLES2/OgreGLES2RenderSystem.h>
#endif

using namespace std;

namespace xio {

//----------------------------------------------------------------------------------------------------------------------
void RenderSystem::InitOgreRenderSystem_GLES2_() {
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#if defined DEBUG || OGRE_PLATFORM == OGRE_PLATFORM_ANDROID || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
  Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLES2RenderSystem());
#endif
#endif
}

}  // namespace xio
