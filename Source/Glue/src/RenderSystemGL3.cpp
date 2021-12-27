// This source file is part of "glue project". Created by Andrew Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include "PCHeader.h"
#include "Engine.h"

#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS

#include <RenderSystems/GL3Plus/OgreGL3PlusRenderSystem.h>

using namespace std;

namespace Glue {

void Engine::InitOgreRenderSystemGL3() {
  auto *GL3Ptr = new Ogre::GL3PlusRenderSystem();
  GL3Ptr->setConfigOption("Separate Shader Objects", "Yes");
  Ogre::Root::getSingleton().setRenderSystem(GL3Ptr);
}

}  // namespace Glue

#endif
