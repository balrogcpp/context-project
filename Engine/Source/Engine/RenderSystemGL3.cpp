// This source file is part of Glue Engine. Created by Andrey Vasiliev
/// \file Compilation failes with both GL3 and GLES2 headers included

#include <OgreComponents.h>


#ifdef OGRE_BUILD_RENDERSYSTEM_GL3PLUS
#include <OgreRoot.h>
#include <RenderSystems/GL3Plus/OgreGL3PlusRenderSystem.h>

namespace Glue {

void InitOgreRenderSystemGL3() { Ogre::Root::getSingleton().setRenderSystem(new Ogre::GL3PlusRenderSystem()); }

bool CheckGL3Version(const int major, const int minor) {
  return static_cast<Ogre::GLRenderSystemCommon *>(Ogre::Root::getSingleton().getRenderSystem())->hasMinGLVersion(major, minor);
}

}  // namespace Glue
#endif  // OGRE_BUILD_RENDERSYSTEM_GL3PLUS
