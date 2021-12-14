// This source file is part of "glue project". Created by Andrew Vasiliev

#include "pch.h"
#include "Engine.h"
/// \file Compilation failes with both GL3 and GLES2 headers included
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
#include <RenderSystems/GL3Plus/OgreGL3PlusRenderSystem.h>
#endif

using namespace std;

namespace Glue {

void Engine::InitOgreRenderSystemGL3() {
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
  auto *GL3Ptr = new Ogre::GL3PlusRenderSystem();
  GL3Ptr->setConfigOption("Separate Shader Objects", "Yes");
  Ogre::Root::getSingleton().setRenderSystem(GL3Ptr);
#endif
}

}  // namespace Glue
