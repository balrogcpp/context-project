// This source file is part of Glue Engine. Created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include "PCHeader.h"
#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
#include "Engine.h"
#include <RenderSystems/GL3Plus/OgreGL3PlusRenderSystem.h>

using namespace std;

namespace Glue {

void Engine::InitOgreRenderSystemGL3() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GL3PlusRenderSystem()); }

bool Engine::CheckGL3Version(int major, int minor) {
  static auto *RS = dynamic_cast<Ogre::GLRenderSystemCommon *>(Ogre::Root::getSingleton().getRenderSystem());
  return RS->hasMinGLVersion(major, minor);
}

}  // namespace Glue

#endif  // OGRE_BUILD_RENDERSYSTEM_GL3PLUS
