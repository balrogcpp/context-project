// This source file is part of Glue Engine. Created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include <OgreComponents.h>


#ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#include <OgreRoot.h>
#include <RenderSystems/GLES2/OgreGLES2RenderSystem.h>

namespace Glue {

void InitOgreRenderSystemGLES2() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLES2RenderSystem()); }

bool CheckGLES2Version(const int major, const int minor) {
  return static_cast<Ogre::GLRenderSystemCommon *>(Ogre::Root::getSingleton().getRenderSystem())->hasMinGLVersion(major, minor);
}

}  // namespace Glue
#endif  // OGRE_BUILD_RENDERSYSTEM_GLES2
