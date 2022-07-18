// This source file is part of Glue Engine. Created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include "PCHeader.h"
#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#include "Engine.h"
#include <RenderSystems/GLES2/OgreGLES2RenderSystem.h>

using namespace std;

namespace Glue {

void Engine::InitOgreRenderSystemGLES2() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLES2RenderSystem()); }

bool Engine::CheckGLES2Version(int major, int minor) {
  static auto *RS = static_cast<Ogre::GLRenderSystemCommon*>(Ogre::Root::getSingleton().getRenderSystem());
  return RS->hasMinGLVersion(major, minor);
}

}  // namespace Glue

#endif  // OGRE_BUILD_RENDERSYSTEM_GLES2
