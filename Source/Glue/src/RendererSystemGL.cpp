// This source file is part of Glue Engine. Created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include "PCHeader.h"
#ifdef OGRE_BUILD_RENDERSYSTEM_GL
#include "Engine.h"
#include <RenderSystems/GL/OgreGLRenderSystem.h>

using namespace std;

namespace Glue {

void Engine::InitOgreRenderSystemGL() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLRenderSystem()); }

bool Engine::CheckGLVersion(int major, int minor) {
  static auto *RS = dynamic_cast<Ogre::GLRenderSystemCommon *>(Ogre::Root::getSingleton().getRenderSystem());
  return RS->hasMinGLVersion(major, minor);
}

}  // namespace Glue

#endif
