/// created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include <OgreComponents.h>

#ifdef OGRE_BUILD_RENDERSYSTEM_GL
#include <OgreRoot.h>
#include <RenderSystems/GL/OgreGLRenderSystem.h>

namespace Glue {

void InitOgreRenderSystemGL() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GLRenderSystem()); }

bool CheckGLVersion(const int major, const int minor) {
  return static_cast<Ogre::GLRenderSystemCommon *>(Ogre::Root::getSingleton().getRenderSystem())->hasMinGLVersion(major, minor);
}

}  // namespace Glue
#endif  // OGRE_BUILD_RENDERSYSTEM_GL
